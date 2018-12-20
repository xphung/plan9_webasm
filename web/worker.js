
let kmodule;
let parent, proc;	// parent and child process
let mem;
let zombie = 0;
let sharedArray;
let sharedLock;




let wasmImport = {
	env: {
		log_writeln: (fd, buffer, n) => console.log(stringAt(buffer, n)),
		tty_read: (fd, buffer, n) => {
			let consumer = Atomics.load(sharedLock, 0);
			while (Atomics.load(sharedLock, 1) == consumer) {
				Atomics.wait(sharedLock, 1, consumer, 1000000);
			}
			let array = new Uint8Array(mem.buffer, buffer, n);
			let j = 0;
			let producer = Atomics.load(sharedLock, 1);
			let size = sharedArray.length;
			while (j < n && consumer < producer) {
				//console.log(consumer+" read="+sharedArray[consumer]);
				array[j++] = sharedArray[(consumer++) % size];
			}
			Atomics.store(sharedLock, 0, consumer);
			return j;
		},
		tty_write: (fd, buffer, n) => {
			var data = stringAt(buffer, n);
			//TODO: translate \n when converting from Uint8 array instead of current separate step
			data = data.replace(/\n/gi, "\r\n");
			sharedLock[2] = buffer;
			sharedLock[3] = n;
			postMessage(data);//t.io.print(data);
			return n;
		},
		clock_gettime: (clock_id, timespec) => {
			return -1;
		},
		osfork: () => {
			if (zombie) throw ("cannot fork");
			const buf  = proc.exports.memory.buffer;
			const mem  = parent.exports.memory
			mem.grow((buf.byteLength - mem.buffer.byteLength) / 65536)
			const dest = new Uint32Array(mem.buffer);
			dest.set(new Uint32Array(buf));
			zombie++;
			return 0;	// thread returns as parent process
		},
		osexec: (cmd, argc, argv) => {
			try {
				const func = proc.exports[cmd+"_main"];
				func(argc, argv);
			} catch (e) {
				if (zombie) {
					const p = proc;
					proc = parent;
					parent = p;
					zombie--;
				}
			}
		},
		osreboot: (cmd, argv) => {
			throw ("reboot: "+cmd);
		},
		_exit: (status) => {
			throw "_exit: status="+status;
		},
		cleanexit: (status) => {
			throw "exit: status="+status;
		},
		sysfault: (buffer, ptr) => {
			var msg = (new TextDecoder().decode(new Uint8Array(mem.buffer, buffer, 30)));
			throw ("sys: "+msg+" "+(ptr|0));
		},
		js_call0: (call_nr) => {
			console.log("sys_call: nr="+call_nr)
			return -1;
		},
		js_call1: (call_nr, a) => {
			console.log("sys_call: nr="+call_nr+" a=0x"+a.toString(16))
			return -1;
		},
		js_call2: (call_nr, a, b) => {
			console.log("sys_call: nr="+call_nr+" a=0x"+a.toString(16)+" b=0x"+b.toString(16))
			return -1;
		},
		js_call3: (call_nr, a, b, c) => {
			console.log("sys_call: nr="+call_nr+" a=0x"+a.toString(16)+" b0x="+b.toString(16)+" c=0x"+c.toString(16))
			return -1;
		},
		js_call4: (call_nr, a, b, c, d) => {
			console.log("sys_call: nr="+call_nr+" a="+a+" b="+b+" c="+c+" d="+d)
			return -1;
		},
		js_call5: (call_nr, a, b, c, d, e) => {
			console.log("sys_call: nr="+call_nr+" a="+a+" b="+b+" c="+c+" d="+d+" e="+e)
			return -1;
		},
		js_call6: (call_nr, a, b, c, d, e, f) => {
			console.log("sys_call: nr="+call_nr+" a="+a+" b="+b+" c="+c+" d="+d+" e="+e+" f="+f)
			return -1;
		},
	}
};

function stringAt(buffer, n) {
	return n > 0 ? (new TextDecoder().decode(new Uint8Array(mem.buffer, buffer, n))) : "";
}

function init() {
	(
	async () => {
		if (!kmodule) {
			const { module, instance } = await WebAssembly.instantiateStreaming(fetch('emu.wasm'), wasmImport);
			kmodule = module;
			parent = instance;
			mem = parent.exports.memory;
			//parent.exports.kmain(0, 0);
		}
		if (!proc) {
			const instance = await WebAssembly.instantiate(kmodule, wasmImport);
			const buffer = parent.exports.memory.buffer;
			mem = instance.exports.memory
			mem.grow((buffer.byteLength - mem.buffer.byteLength) / 65536)
			const ubytes = new Uint32Array(mem.buffer);
			ubytes.set(new Uint32Array(buffer));
			proc = instance;
			proc.exports._start();
		}
		console.log('Worker: Posting message back to main script\n');
		postMessage(count);
	})();

}

onmessage = function(m) {
	sharedArray = new Uint8Array(m.data, 32);
	sharedLock = new Int32Array(m.data, 0, 32);
	console.log('[WORKER] Received SharedArrayBuffer ' + m.data)
	init();

}
