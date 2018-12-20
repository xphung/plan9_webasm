#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	"version.h"
#include	"mp.h"
#include	"libsec.h"
#include	"keyboard.h"

enum
{
	Qdir,
	Qcons,
	Qconsctl,
	Qdrivers,
	Qhostdomain,
	Qhostowner,
	Qhoststdin,
	Qhoststdout,
	Qhoststderr,
	Qkeyboard,
	Qkprint,
	Qmemory,
	Qnull,
	Qosversion,
	Qpgrpid,
	Qpid,
	Qppid,
	Qrandom,
	Qreboot,
	Qscancode,
	Qsysctl,
	Qsysname,
	Qtime,
	Quser,
	Qzero
};

Dirtab consdir[] =
{
	".",	{Qdir, 0, QTDIR},	0,		DMDIR|0555,
	"cons",		{Qcons},	0,		0666,
	"consctl",	{Qconsctl},	0,		0222,
	"drivers",	{Qdrivers},	0,		0444,
	"hostowner",	{Qhostowner},	0,		0644,
	"hostdomain",	{Qhostdomain},	DOMLEN,		0664,
	"hoststdin",	{Qhoststdin},	0,		0444,
	"hoststdout",	{Qhoststdout},	0,		0222,
	"hoststderr",	{Qhoststderr},	0,		0222,
	"keyboard",	{Qkeyboard},	0,		0666,
	"kprint",	{Qkprint},	0,		0444,
	"memory",	{Qmemory},	0,		0444,
	"null",		{Qnull},	0,		0666,
	"osversion",	{Qosversion},	0,		0444,
	"pgrpid",	{Qpgrpid},	NUMSIZE,	0444,
	"pid",		{Qpid},		NUMSIZE,	0444,
	"ppid",		{Qppid},	NUMSIZE,	0444,
	"random",	{Qrandom},	0,		0444,
	"reboot",	{Qreboot},	0,		0660,
	"scancode",	{Qscancode},	0,		0444,
	"sysctl",	{Qsysctl},	0,		0644,
	"sysname",	{Qsysname},	0,		0644,
	"time",		{Qtime},	0,		0644,
	"user",		{Quser},	0,		0644,
	"zero",		{Qzero},	0,		0444,
};

extern	int	cflag;
extern	int	dflag;

char	*ossysname;
vlong	timeoffset;

Queue*	gkscanq;		/* Graphics keyboard raw scancodes */
char*	gkscanid;		/* name of raw scan format (if defined) */
Queue*	gkbdq;			/* Graphics keyboard unprocessed input */
Queue*	lineq;			/* processed console input */
static struct
{
	RWlock l;
	Queue*	q;
}	kprintq;

static struct
{
	QLock	q;
	QLock	gq;		/* separate lock for the graphical input */

	int	raw;		/* true if we shouldn't process input */
	Ref	ctl;		/* number of opens to the control file */
	Ref	ptr;		/* number of opens to the ptr file */
	int	scan;		/* true if reading raw scancodes */
	int	x;		/* index into line */
	char	line[1024];	/* current input line */

	Rune	c;
	int	count;
} kbd;

enum
{
	CMhalt,
	CMreboot,
	CMpanic,
};

Cmdtab rebootmsg[] =
{
	CMhalt,		"halt",		1,
	CMreboot,	"reboot",	0,
	CMpanic,	"panic",	0,
};

void
gkbdputc(Queue *q, int ch)
{
	int n;
	Rune r;
	static uchar kc[5*UTFmax];
	static int nk, collecting = 0;
	char buf[UTFmax];

	r = ch;
	if(r == Latin) {
		collecting = 1;
		nk = 0;
		return;
	}
	if(collecting) {
		long c;
		nk += runetochar((char*)&kc[nk], &r);
		c = latin1(kc, nk);
		if(c < -1)	/* need more keystrokes */
			return;
		collecting = 0;
		if(c == -1) {	/* invalid sequence */
			qproduce(q, kc, nk);
			return;
		}
		r = (Rune)c;
	}
	n = runetochar(buf, &r);
	if(n == 0)
		return;
	/* if(!isdbgkey(r)) */ 
		qproduce(q, buf, n);
}

void
consinit(void)
{
	lineq = qopen(2*1024, 0, nil, nil);
	if(lineq == 0)
		panic("no memory");
	gkbdq = qopen(512, 0, nil, nil);
	if(gkbdq == 0)
		panic("no memory");
	randominit();
}

static Chan*
consattach(char *spec)
{
	return devattach('c', spec);
}

static Walkqid*
conswalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, consdir, nelem(consdir), devgen);
}

static int
consstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, consdir, nelem(consdir), devgen);
}

static Chan*
consopen(Chan *c, int omode)
{
	c = devopen(c, omode, consdir, nelem(consdir), devgen);
	switch((ulong)c->qid.path) {
	case Qconsctl:
		incref(&kbd.ctl);
		break;

	case Qscancode:
		qlock(&kbd.gq);
		if(gkscanq != nil || gkscanid == nil) {
			qunlock(&kbd.q);
			c->flag &= ~COPEN;
			if(gkscanq)
				error(Einuse);
			else
				error("not supported");
		}
		gkscanq = qopen(256, 0, nil, nil);
		qunlock(&kbd.gq);
		break;

	case Qkprint:
		wlock(&kprintq.l);
		if(waserror()){
			wunlock(&kprintq.l);
			c->flag &= ~COPEN;
			nexterror();
		}
		if(kprintq.q != nil)
			error(Einuse);
		kprintq.q = qopen(32*1024, Qcoalesce, nil, nil);
		if(kprintq.q == nil)
			error(Enomem);
		qnoblock(kprintq.q, 1);
		poperror();
		wunlock(&kprintq.l);
		c->iounit = qiomaxatomic;
		break;
	}
	return c;
}

static void
consclose(Chan *c)
{
	if((c->flag & COPEN) == 0)
		return;

	switch((ulong)c->qid.path) {
	case Qconsctl:
		/* last close of control file turns off raw */
		if(decref(&kbd.ctl) == 0)
			kbd.raw = 0;
		break;

	case Qscancode:
		qlock(&kbd.gq);
		if(gkscanq) {
			qfree(gkscanq);
			gkscanq = nil;
		}
		qunlock(&kbd.gq);
		break;

	case Qkprint:
		wlock(&kprintq.l);
		qfree(kprintq.q);
		kprintq.q = nil;
		wunlock(&kprintq.l);
		break;
	}
}

static long
consread(Chan *c, void *va, long n, vlong offset)
{
	int send;
	char *p, buf[64], ch;

	if(n <= 0)
		return n;

	switch((ulong)c->qid.path){
	case Qdir:
		return devdirread(c, va, n, consdir, nelem(consdir), devgen);

	case Qcons:
		if(kbd.raw)
			return tty_read(0, (long)va, n);

		if(dflag)
			error(Enonexist);

		while(!qcanread(lineq)) {
			size_t r = tty_read(0, (long)(&ch), 1);
			if(r<0) error("read stdin"); else if(!r) ch = 0x04; //if(!r) continue;
			send = 0;

			if(ch == 0){
				/* flush output on rawoff -> rawon */
				if(kbd.x > 0)
					send = 1; //!qcanread(kbdq);
			}else{
				switch(ch){
				case 0x7f:
				case '\b':
					if(kbd.x) {
						kbd.x--;
						tty_write(1, (long)("\b \b"), 3);
					}
					break;
				case 0x15:
					kbd.x = 0;
					tty_write(1, (long)("^U\n"), 3);
					break;
				case 0x1c:
					exits("SIGQUIT");
					break;
				case 0x04:
					send = 1;
					break;
				case '\r':
					ch = '\n';
				case '\n':
					send = 1;
				default:
					kbd.line[kbd.x++] = ch;
					tty_write(1, (long)(&ch), 1);
					break;
				}
			}
			if(send || kbd.x == sizeof kbd.line){
				qwrite(lineq, kbd.line, kbd.x);
				kbd.x = 0;
			}
		}
		n = qread(lineq, va, n);
		qunlock(&kbd.q);
		poperror();
		return n;

	case Qscancode:
		if(offset == 0)
			return readstr(0, va, n, gkscanid);
		return qread(gkscanq, va, n);

	case Qkeyboard:
		return qread(gkbdq, va, n);

	case Qkprint:
		rlock(&kprintq.l);
		if(waserror()){
			runlock(&kprintq.l);
			nexterror();
		}
		n = qread(kprintq.q, va, n);
		poperror();
		runlock(&kprintq.l);
		return n;

	case Qpgrpid:
		return readnum((ulong)offset, va, n, up->env->pgrp->pgrpid, NUMSIZE);

	case Qpid:
		return readnum((ulong)offset, va, n, up->pid, NUMSIZE);

	case Qppid:
		return -1; //readnum((ulong)offset, buf, n, up->parentpid, NUMSIZE);

	case Qtime:
		snprint(buf, sizeof(buf), "%.lld", timeoffset + nsec() / 1000);
		return readstr(offset, va, n, buf);

	case Qhostowner:
		return readstr(offset, va, n, eve);

	case Qhostdomain:
		return readstr((ulong)offset, va, n, hostdomain);

	case Qhoststdin:
		return tty_read(0, (long)va, n);	/* should be pread */

	case Quser:
		return readstr(offset, va, n, up->env->user);

	case Qnull:
		return 0;

	case Qsysctl:
		return readstr(offset, va, n, VERSION);

	case Qsysname:
		if(ossysname == nil)
			return 0;
		return readstr(offset, va, n, ossysname);

	case Qrandom:
		return randomread(va, n);

	case Qdrivers:
		return devtabread(c, va, n, offset);

	case Qmemory:
		return poolread(va, n, offset);

	case Qzero:
		memset(va, 0, n);
		return n;

	case Qosversion:
		snprint(buf, sizeof buf, "2000");
		n = readstr((ulong)offset, va, n, buf);
		return n;

	default:
		print("consread %#llux\n", c->qid.path);
		error(Egreg);
	}
	return -1;		/* never reached */
	
}

static long
conswrite(Chan *c, void *va, long n, vlong offset)
{
	char buf[256], ch;
	char *a = (char *)va;
	int x;
	Cmdbuf *cb;
	Cmdtab *ct;

	if(c->qid.type & QTDIR)
		error(Eperm);

	switch((ulong)c->qid.path) {
	case Qcons:
		if(canrlock(&kprintq.l)){
			if(kprintq.q != nil){
				if(waserror()){
					runlock(&kprintq.l);
					nexterror();
				}
				qwrite(kprintq.q, va, n);
				poperror();
				runlock(&kprintq.l);
				return n;
			}
			runlock(&kprintq.l);
		}
		return tty_write(1, (long)va, n);
		
	case Qconsctl:
		if(n >= sizeof(buf))
			n = sizeof(buf)-1;
		strncpy(buf, va, n);
		buf[n] = 0;
		for(a = buf; a;){
			if(strncmp(a, "rawon", 5) == 0){
				kbd.raw = 1;
				ch = 0;
				//qwrite(kbdq, &ch, 1);  /* wake up reader not required as consdev is now single threaded */
			} else if(strncmp(buf, "rawoff", 6) == 0){
				kbd.raw = 0;
			}
			if((a = strchr(a, ' ')) != nil)
				a++;
		}
		break;

	case Qkeyboard:
		for(x=0; x<n; ) {
			Rune r;
			x += chartorune(&r, &((char*)va)[x]);
			gkbdputc(gkbdq, r);
		}
		break;

	case Qtime:
		if(n >= sizeof(buf))
			n = sizeof(buf)-1;
		strncpy(buf, va, n);
		buf[n] = '\0';
		timeoffset = strtoll(buf, 0, 0)- (nsec()/1000);
		break;

	case Qhostowner:
		return hostownerwrite(a, n);

	case Qhostdomain:
		return hostdomainwrite(a, n);

	case Qhoststdout:
		return tty_write(1, (long)va, n);

	case Qhoststderr:
		return tty_write(2, (long)va, n);

	case Quser:
		if(!iseve())
			error(Eperm);
		if(offset != 0)
			error(Ebadarg);
		if(n <= 0 || n >= sizeof(buf))
			error(Ebadarg);
		strncpy(buf, va, n);
		buf[n] = '\0';
		if(n > 0 && buf[n-1] == '\n')
			buf[--n] = '\0';
		if(n == 0)
			error(Ebadarg);
		panic("/dev/user"); //setid(buf, 0);
		break;

	case Qnull:
		break;

	case Qsysctl:
	case Qreboot:
		if(!iseve())
			error(Eperm);
		cb = parsecmd(a, n);

		if(waserror()) {
			free(cb);
			nexterror();
		}
		ct = lookupcmd(cb, rebootmsg, nelem(rebootmsg));
		switch(ct->index) {
		case CMhalt:
			if(cb->nf > 1)
				x = (int) strtoll(cb->f[1], 0, 0);
			else
				x = 0;
			cleanexit(x);		/* XXX ignored for the time being (and should be a string anyway) */
			break;
		case CMreboot:
			osreboot(cb->f[1], cb->f+1);
			break;
		case CMpanic:
			panic("/dev/reboot");
		}
		poperror();
		free(cb);
		break;

	case Qsysname:
		if(offset != 0)
			error(Ebadarg);
		if(n < 0 || n >= sizeof(buf))
			error(Ebadarg);
		strncpy(buf, va, n);
		buf[n] = '\0';
		if(buf[n-1] == '\n')
			buf[n-1] = 0;
		kstrdup(&ossysname, buf);
		break;

	default:
		print("conswrite: %#llux\n", c->qid.path);
		error(Egreg);
	}
	return n;
}

Dev consdevtab = {
	'c',
	"cons",

	consinit,
	consattach,
	conswalk,
	consstat,
	consopen,
	devcreate,
	consclose,
	consread,
	devbread,
	conswrite,
	devbwrite,
	devremove,
	devwstat
};

static	ulong	randn;

static void
seedrand(void)
{
	randomread((void*)&randn, sizeof(randn));
}

int
nrand(int n)
{
	if(randn == 0)
		seedrand();
	randn = randn*1103515245 + 12345 + nsec()/1000;
	return (randn>>16) % n;
}

int
rand(void)
{
	nrand(1);
	return randn;
}

ulong
truerand(void)
{
	ulong x;

	randomread(&x, sizeof(x));
	return x;
}

QLock grandomlk;

void
_genrandomqlock(void)
{
	qlock(&grandomlk);
}


void
_genrandomqunlock(void)
{
	qunlock(&grandomlk);
}
