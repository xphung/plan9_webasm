#include	"dat.h"
#include	"fns.h"
#include	"error.h"
#include	<interp.h>

/*
 * Enable the heap device for environments that allow debugging =>
 * Must be 1 for a production environment.
 */
int	SECURE = 0;

enum
{
	Qdir,
	Qctl,
	Qdbgctl,
	Qheap,
	Qns,
	Qnsgrp,
	Qpgrp,
	Qproc,
	Qstack,
	Qstatus,
	Qtext,
	Qwait,
	Qfd,
	Qexception,
};

enum
{
	CMclose,
	CMclosefiles,
	CMhang,
	CMnohang,
	CMkill,
	CMkillgrp,
	CMrestricted,
	CMexceptions,
	CMprivate
};

/*
 * Status, fd, and ns are left readable (0400/0444) because of their use in debugging,
 * particularly on shared servers.
 * Arguably, ns and fd shouldn't be readable; if you'd prefer, change them to 0000
 */
Dirtab procdir[] =
{
	"ctl",		{Qctl},		0,			0200,
	"dbgctl",	{Qdbgctl},	0,			0600,
	"fd",		{Qfd},		0,			0400,
	"heap",		{Qheap},	0,			0600,
	"ns",		{Qns},		0,			0400,
	"nsgrp",	{Qnsgrp},	0,			0444,
	"pgrp",		{Qpgrp},	0,			0444,
	"proc",		{Qproc},	0,			0400,
	"stack",	{Qstack},	0,			0400,
	"status",	{Qstatus},	0,			0444,
	"text",		{Qtext},	0,			0000,
	"wait",		{Qwait},	0,			0400,
	"exception",	{Qexception},	0,			0400,
};

static
Cmdtab proccmd[] = {
	CMclose,		"close",		2,
	CMclosefiles,		"closefiles",		1,
	CMkill,			"kill",			1,
	CMkillgrp,		"killgrp",		1,
	CMprivate,		"private",		1,
	CMrestricted,		"restricted",		1,
	CMexceptions,		"exceptions",		2,
};

/*
 * Qids are, in path:
 *	 	5 bits of file type (qids above)
 *		59? bits of (pid + 1)
 *	     in vers:
 *		TODO: 32 bits of pid, for consistency checking
 * If notepg, c->pgrpid.path is pgrp slot, .vers is noteid.
 */
#define	QSHIFT		5		/* location in qid of pid */
#define	QID(q)		((((ulong)(q).path) & ((1<<QSHIFT)-1)) >> 0)
#define QPID(pid)	(((pid+1)<<QSHIFT))
#define	PID(q)		((q).vers)
#define PATH(q)		((ulong)(q).path&~((1<<QSHIFT)-1))

static char *procstate[] =			/* must correspond to include/interp.h */
{
	"alt",				/* blocked in alt instruction */
	"send",				/* waiting to send */
	"recv",				/* waiting to recv */
	"debug",			/* debugged */
	"ready",			/* ready to be scheduled */
	"release",			/* interpreter released */
	"exiting",			/* exit because of kill or error */
	"broken",			/* thread crashed */
};

static	void	procctlreq(Proc*, char*, int);
static	void	mntscan(Mntwalk*, Pgrp*);
static  char 	Emisalign[] = "misaligned address";

static int
procgen(Chan *c, char *name, Dirtab *tab, int ntab, int s, Dir *dp)
{
	Qid qid;
	Proc *p;
	char *e;
	Osenv *o;
	ulong pid, path, perm, len;

	USED(ntab);

	if(s == DEVDOTDOT){
		mkqid(&qid, Qdir, 0, QTDIR);
		devdir(c, qid, "#p", 0, eve, DMDIR|0555, dp);
		return 1;
	}

	if((ulong)c->qid.path == Qdir) {
		if(name != nil){
			/* ignore s and use name to find pid */
			pid = strtoul(name, &e, 0);
			if(/* pid == 0 || */ *e != '\0')
				return -1;
			acquire();
			p = procpid(pid);
			if(p == nil){
				release();
				return -1;
			}
		}else{
			acquire();
			p = proctab(s);
			if(p == nil) {
				release();
				return -1;
			}
			pid = p->pid;
		}
		o = p->env;
		sprint(up->genbuf, "%lud", pid);
		if(name != nil && strcmp(name, up->genbuf) != 0){
			release();
			return -1;
		}
		mkqid(&qid, (pid+1)<<QSHIFT, pid, QTDIR);
		devdir(c, qid, up->genbuf, 0, o->user, DMDIR|0555, dp);
		release();
		return 1;
	}

	if(s >= nelem(procdir))
		return -1;
	tab = &procdir[s];
	path = PATH(c->qid);

	acquire();
	p = procpid(PID(c->qid));
	if(p == nil) {
		release();
		return -1;
	}

	o = p->env;

	perm = tab->perm;
	if((perm & 7) == 0)
		perm = (perm|(perm>>3)|(perm>>6)) & o->pgrp->progmode;
	
	len = tab->length;
	mkqid(&qid, path|tab->qid.path, c->qid.vers, QTFILE);
	devdir(c, qid, tab->name, len, o->user, perm, dp);
	release();
	return 1;
}

static Chan*
procattach(char *spec)
{
	return devattach('p', spec);
}

static Walkqid*
procwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, 0, 0, procgen);
}

static int
procstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, 0, 0, procgen);
}

/*
 *  none can't read or write state on other
 *  processes.  This is to contain access of
 *  servers running as none should they be
 *  subverted by, for example, a stack attack.
 */
static void
nonone(Proc *p)
{
	if(p == up)
		return;
	if(strcmp(up->env->user, "none") != 0)
		return;
	if(iseve())
		return;
	error(Eperm);
}

static Chan*
procopen(Chan *c, int omode)
{
	Proc *p;
	Osenv *o;
	long perm;

	if(c->qid.type & QTDIR)
		return devopen(c, omode, 0, 0, procgen);

	acquire();
	if (waserror()) {
		release();
		nexterror();
	}
	p = procpid(PID(c->qid));
	if(p == nil)
		error(Ethread);
	o = p->env;
	perm = procdir[QID(c->qid)-1].perm;
	if((perm & 7) == 0)
		perm = (perm|(perm>>3)|(perm>>6)) & o->pgrp->progmode;
	devpermcheck(o->user, perm, omode);
	omode = openmode(omode);

	switch(QID(c->qid)){
	case Qexception:
		break;
		
	case Qproc:
	case Qfd:
		if(omode != OREAD)
			error(Eperm);
		break;

	case Qctl:
		//if(p->privatemem)
		//	error(Eperm);
		nonone(p);
		break;

	case Qnsgrp:
	case Qpgrp:
	case Qstatus:
		nonone(p);
		break;

	case Qwait:
		c->aux = qopen(1024, Qmsg, nil, nil);
		if(c->aux == nil)
			error(Enomem);
		o->childq = c->aux;
		break;

	case Qns:
		if(omode != OREAD)
			error(Eperm);

		if(c->aux == nil)
			error(Enomem);
		break;
	default:
		print("procopen %#lux\n", QID(c->qid));
		error(Egreg);
	}
	//if(p->state != Pexiting)
	c->qid.vers = p->pid;
	coherence();

	poperror();
	release();
	c->offset = 0;
	c->mode = omode;
	c->flag |= COPEN;
	return c;
}

static int
procwstat(Chan *c, uchar *db, int n)
{
	Dir d;
	Proc *p;
	char *u;
	Osenv *o;

	if(c->qid.type&QTDIR)
		error(Eperm);
	acquire();
	p = procpid(PID(c->qid));
	if(p == nil) {
		release();
		error(Ethread);
	}

	u = up->env->user;
	o = p->env;
	if(strcmp(u, o->user) != 0 && strcmp(u, eve) != 0) {
		release();
		error(Eperm);
	}

	n = convM2D(db, n, &d, nil);
	if(n == 0){
		release();
		error(Eshortstat);
	}
	if(d.mode != ~0UL)
		o->pgrp->progmode = d.mode&0777;
	release();
	return n;
}

static int
procsize(Proc *p)
{
	return 0 / 1024;
}

static long
procoffset(long offset, char *va, int *np)
{
	if(offset > 0) {
		offset -= *np;
		if(offset < 0) {
			memmove(va, va+*np+offset, -offset);
			*np = -offset;
		}
		else
			*np = 0;
	}
	return offset;
}

static int
procqidwidth(Chan *c)
{
	char buf[32];

	return sprint(buf, "%lud", c->qid.vers);
}

int
procfdprint(Chan *c, int fd, int w, char *s, int ns)
{
	int n;

	if(w == 0)
		w = procqidwidth(c);
	n = snprint(s, ns, "%3d %.2s %C %4ld (%.16llux %*lud %.2ux) %5ld %8lld %s\n",
		fd,
		&"r w rw"[(c->mode&3)<<1],
		devtab[c->type]->dc, c->dev,
		c->qid.path, w, c->qid.vers, c->qid.type,
		c->iounit, c->offset, c->name->s);
	return n;
}

static int
procfds(Proc *p, char *va, int count, long offset)
{
	Fgrp *f;
	Chan *c;
	int n, i, w, ww;
	char a[256];

	/* print to buf to avoid holding fgrp lock while writing to user space */
	if(count > sizeof a)
		count = sizeof a;

	acquire();  //qlock(&p->debug);
	f = p->env->fgrp;	/* f is not locked because we've acquired */
	if(f == nil){
		release(); //qunlock(&p->debug);
		return 0;
	}
	lock(&f->l);
	if(waserror()){
		unlock(&f->l);
		release(); //qunlock(&p->debug);
		nexterror();
	}
	
	n = readstr(0, a, count, p->env->pgrp->dot->name->s);
	n += snprint(a+n, count-n, "\n");
	offset = procoffset(offset, a, &n);
	/* compute width of qid.path */
	w = 0;
	for(i = 0; i <= f->maxfd; i++) {
		c = f->fd[i];
		if(c == nil)
			continue;
		ww = procqidwidth(c);
		if(ww > w)
			w = ww;
	}
	for(i = 0; i <= f->maxfd; i++) {
		c = f->fd[i];
		if(c == nil)
			continue;
		n += procfdprint(c, i, w, a+n, count-n);
		offset = procoffset(offset, a, &n);
	}
	unlock(&f->l);
	release();  //qunlock(&p->debug);
	poperror();

	/* copy result to user space, now that locks are released */
	memmove(va, a, n);

	return n;
}

static void
procclose(Chan *c)
{
	int i;
	Proc *f;
	Osenv *o;

	switch(QID(c->qid)) {
	case Qns:
	case Qheap:
		free(c->aux);
		break;
		
	case Qwait:
		acquire();
		i = 0;
		for(;;) {
			f = proctab(i++);
			if(f == nil)
				break;
			o = f->env;
			if(o->waitq == c->aux)
				o->waitq = nil;
			if(o->childq == c->aux)
				o->childq = nil;
		}
		release();
		qfree(c->aux);
	}
}

static void
int2flag(int flag, char *s)
{
	if(flag == 0){
		*s = '\0';
		return;
	}
	*s++ = '-';
	if(flag & MAFTER)
		*s++ = 'a';
	if(flag & MBEFORE)
		*s++ = 'b';
	if(flag & MCREATE)
		*s++ = 'c';
	if(flag & MCACHE)
		*s++ = 'C';
	*s = '\0';
}

static char*
proctime(ulong msec, char *buf, char *ebuf)
{
	int tenths, sec;

	tenths = msec/100;
	sec = tenths/10;
	seprint(buf, ebuf, "%4d:%2.2d.%d", sec/60, sec%60, tenths%10);
	return buf;
}

static long
procread(Chan *c, void *va, long n, vlong offset)
{
	char *a = va;
	char flag[10];
	ulong grpid;
	int i;
	char mbuf[64], timebuf[12];
	Mntwalk *mw;
	Osenv *o;
	Proc *p;

	if(c->qid.type & QTDIR)
		return devdirread(c, a, n, 0, 0, procgen);

	p = procpid(PID(c->qid));

	switch(QID(c->qid)){
	case Qproc:
		if(offset >= sizeof(Proc))
			return 0;
		if(offset+n > sizeof(Proc))
			n = sizeof(Proc) - offset;
		memmove(a, ((char*)p)+offset, n);
		return n;
		
	case Qstatus:
		acquire();
		p = procpid(PID(c->qid));
		if(p == nil /* || p->state == Pexiting || p->R.M == H */) {
			release();
			snprint(up->genbuf, sizeof(up->genbuf), "%8lud %8d %10s %s %10s %5dK %s",
				PID(c->qid),
				0,
				eve,
				proctime(0, timebuf, timebuf+sizeof(timebuf)),
				procstate[Pexiting],
				0,
				"[$Sys]");
			return readstr(offset, va, n, up->genbuf);
		}
		mbuf[0] = (char)0; //modstatus(&p->R, mbuf, sizeof(mbuf));
		o = p->env;
		snprint(up->genbuf, sizeof(up->genbuf), "%8d %8d %10s %s %10s %5dK %s",
			p->pid,
			0, //p->group!=nil? p->group->id: 0,
			o->user,
			proctime(0 /*p->ticks*/, timebuf, timebuf+sizeof(timebuf)),
			0, //procstate[p->state],
			procsize(p),
			mbuf);
		release();
		return readstr(offset, va, n, up->genbuf);
		
	case Qwait:
		return qread(c->aux, va, n);
		
	case Qns:
		acquire();
		if(waserror()){
			release();
			nexterror();
		}
		p = procpid(PID(c->qid));
		if(p == nil)
			error(Ethread);
		mw = c->aux;
		if(mw->cddone){
			poperror();
			release();
			return 0;
		}
		o = p->env;
		mntscan(mw, o->pgrp);
		if(mw->mh == 0) {
			mw->cddone = 1;
			i = snprint(a, n, "cd %s\n", o->pgrp->dot->name->s);
			poperror();
			release();
			return i;
		}
		int2flag(mw->cm->mflag, flag);
		if(strcmp(mw->cm->to->name->s, "#M") == 0){
			i = snprint(a, n, "mount %s %s %s %s\n", flag,
				mw->cm->to->mchan->name->s,
				mw->mh->from->name->s, mw->cm->spec? mw->cm->spec : "");
		}else
			i = snprint(a, n, "bind %s %s %s\n", flag,
				mw->cm->to->name->s, mw->mh->from->name->s);
		poperror();
		release();
		return i;
		
	case Qnsgrp:
		acquire();
		p = procpid(PID(c->qid));
		if(p == nil) {
			release();
			error(Ethread);
		}
		grpid = ((Osenv *)p->env)->pgrp->pgrpid;
		release();
		return readnum(offset, va, n, grpid, NUMSIZE);
		
	case Qpgrp:
		acquire();
		p = procpid(PID(c->qid));
		if(p == nil) {
			release();
			error(Ethread);
		}
		grpid = -1;  //p->group!=nil? p->group->id: 0;
		release();
		return readnum(offset, va, n, grpid, NUMSIZE);

	case Qfd:
		p = procpid(PID(c->qid));
		return procfds(p, va, n, offset);
		
	case Qexception:
		acquire();
		p = procpid(PID(c->qid));
		if(p == nil) {
			release();
			error(Ethread);
		}
		if(p->nerr == 0)
			up->genbuf[0] = 0;
		else
			snprint(up->genbuf, sizeof(up->genbuf), p->env->errstr);
		release();
		return readstr(offset, va, n, up->genbuf);
	}
	error(Egreg);
	return 0;
}

static void
mntscan(Mntwalk *mw, Pgrp *pg)
{
	Mount *t;
	Mhead *f;
	int nxt, i;
	ulong last, bestmid;

	rlock(&pg->ns);

	nxt = 0;
	bestmid = ~0;

	last = 0;
	if(mw->mh)
		last = mw->cm->mountid;

	for(i = 0; i < MNTHASH; i++) {
		for(f = pg->mnthash[i]; f; f = f->hash) {
			for(t = f->mount; t; t = t->next) {
				if(mw->mh == 0 ||
				  (t->mountid > last && t->mountid < bestmid)) {
					mw->cm = t;
					mw->mh = f;
					bestmid = mw->cm->mountid;
					nxt = 1;
				}
			}
		}
	}
	if(nxt == 0)
		mw->mh = 0;

	runlock(&pg->ns);
}

static long
procwrite(Chan *c, void *va, long n, vlong offset)
{
	Proc *p;

	USED(offset);
	USED(va);

	if(c->qid.type & QTDIR)
		error(Eisdir);

	acquire();  //qlock(&p->debug);
	if(waserror()) {
		release();
		nexterror();
	}
	p = procpid(PID(c->qid));
	if(p == nil)
		error(Ethread);

	switch(QID(c->qid)){
	case Qctl:
		procctlreq(p, va, n);
		break;

	default:
		print("unknown qid in procwrite\n");
		error(Egreg);
	}
	poperror();
	release();  //qunlock(&p->debug);
	return n;
}

Dev procdevtab = {
	'p',
	"proc",

	devinit,
	procattach,
	procwalk,
	procstat,
	procopen,
	devcreate,
	procclose,
	procread,
	devbread,
	procwrite,
	devbwrite,
	devremove,
	procwstat
};

static void
procctlcloseone(Proc *p, Fgrp *f, int fd)
{
	Chan *c;

	c = f->fd[fd];
	if(c == nil)
		return;
	f->fd[fd] = nil;
	unlock(&f->l);
	release();  //qunlock(&p->debug);
	cclose(c);
	acquire();  //qlock(&p->debug);
	lock(&f->l);
}

void
procctlclosefiles(Proc *p, int all, int fd)
{
	int i;
	Fgrp *f;

	f = p->env->fgrp;
	if(f == nil)
		error(Ethread);

	lock(&f->l);
	f->r.ref++;
	if(all)
		for(i = 0; i < f->maxfd; i++)
			procctlcloseone(p, f, i);
	else
		procctlcloseone(p, f, fd);
	unlock(&f->l);
	closefgrp(f);
}

void
procctlreq(Proc *p, char *va, int n)
{
	int npc, pri;
	Cmdbuf *cb;
	Cmdtab *ct;
	vlong time;
	char *e;
	void (*pt)(Proc*, int, vlong);

	if(p->type)	/* no ctl requests to kprocs (repurpose type = kp) */
		error(Eperm);

	cb = parsecmd(va, n);
	if(waserror()){
		free(cb);
		nexterror();
	}

	ct = lookupcmd(cb, proccmd, nelem(proccmd));

	switch(ct->index){
	case CMclose:
		procctlclosefiles(p, 0, atoi(cb->f[1]));
		break;
	case CMclosefiles:
		procctlclosefiles(p, 1, 0);
		break;
	case CMkillgrp:
			//killgrp(p, "killed");
			break;
	case CMkill:
			//killprog(p, "killed");
			break;
	}

	poperror();
	free(cb);
}

