# plan9_webasm
WebAssembly port of Plan9 C libraries, devices, filesystems, compiler, command line tools and Inferno kernel. 
(I believe this is first Unix-derived operating system to be ported to WebAssembly!)

musl (C library subset) and syscall stubs into Inferno kernel also being ported.

Top level source directories are now:

-  musl

-  sys

-  wasm/musl

-  wasm/sys

Expects to have musl 1.1.20 in ./musl

WebAssembly specific files in ./wasm/musl tree overrides corresponding source file in ./musl tree

Likewise ./wasm/sys overrides  corresponding source file in ./sys tree