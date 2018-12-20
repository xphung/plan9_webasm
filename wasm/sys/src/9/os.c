/*
 * Loosely based on FreeBSD/os.c and Solaris/os.c
 * Copyright © 1998, 1999 Lucent Technologies Inc.  All rights reserved.
 * Revisions Copyright © 1999, 2000 Vita Nuova Limited.  All rights reserved.
 * Revisions Copyright © 2002, 2003 Corpus Callosum Corporation.  All rights reserved.
 */
#include "dat.h"
#include "fns.h"
#include "error.h"
#include "pthread.h"
#include "raise.h"
#include <tos.h>
#include "pwd.h"

Procs	procs;
char	*hosttype = "POSIX";
char	*cputype = "Wasm32";

/* Structure used for os specific data: */
typedef struct Sem Sem;
struct Sem {
	pthread_cond_t	c;
	pthread_mutex_t	m;
	int	v;
  int proc_errno;
};

static pthread_key_t  prdakey;

extern int dflag;

struct	Plink
{
	Plink	*old;
	Plink	*down;
	Plink	*link;
	long	pc;
	long	count;
	vlong	time;
};

Tos _inittos;
Tos *_tos = &_inittos;

Proc*
getup(void)
{
	return (Proc *)pthread_getspecific(prdakey);
}

void
pexit(char *msg, int t)
{
	Osenv *e;
	Proc *p;
	Sem *sem;

	USED(t);
	USED(msg);

	lock(&procs.l);
	p = up;
	if(p->prev)
		p->prev->next = p->next;
	else
		procs.head = p->next;

	if(p->next)
		p->next->prev = p->prev;
	else
		procs.tail = p->prev;
	unlock(&procs.l);

	if(0)
		print("pexit: %s: %s\n", p->text, msg);

	e = p->env;
	if(e != nil) {
		closefgrp(e->fgrp);
		closepgrp(e->pgrp);
		closeegrp(e->egrp);
		closesigs(e->sigs);
		free(e->user);
	}
	free(p->prog);
	sem = p->os;
	if(sem != nil){
		//pthread_cond_destroy(&sem->c);
		//pthread_mutex_destroy(&sem->m);
	}
	free(p->os);
	free(p);
	print("pexit: %s: %s\n", p->text, msg);
	_exits("termination by pexit\n"); //pthread_exit(0);
}

void
kproc(char *name, void (*func)(void*), void *arg, int flags)
{
	//pthread_t thread;
	Proc *p;
	Pgrp *pg;
	Fgrp *fg;
	Egrp *eg;
	//pthread_attr_t attr;
	Sem *sem;

	p = newproc();
	if(p == nil)
		panic("kproc: no memory");
	sem = malloc(sizeof(*sem));
	if(sem == nil)
		panic("can't allocate semaphore");
	//pthread_cond_init(&sem->c, NULL);
	//pthread_mutex_init(&sem->m, NULL);
	sem->v = 0;
	p->os = sem;

	if(flags & KPDUPPG) {
		pg = up->env->pgrp;
		incref(&pg->r);
		p->env->pgrp = pg;
	}
	if(flags & KPDUPFDG) {
		fg = up->env->fgrp;
		incref(&fg->r);
		p->env->fgrp = fg;
	}
	if(flags & KPDUPENVG) {
		eg = up->env->egrp;
		incref(&eg->r);
		p->env->egrp = eg;
	}

	p->env->uid = up->env->uid;
	p->env->gid = up->env->gid;
	kstrdup(&p->env->user, up->env->user);

	strcpy(p->text, name);

	p->func = func;
	p->arg = arg;

	lock(&procs.l);
	if(procs.tail != nil) {
		p->prev = procs.tail;
		procs.tail->next = p;
	} else {
		procs.head = p;
		p->prev = nil;
	}
	procs.tail = p;
	unlock(&procs.l);
  
#if 0
	if(pthread_attr_init(&attr) == -1)
		panic("pthread_attr_init failed");

	pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
	pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if(pthread_create(&thread, &attr, tramp, p))
		panic("thr_create failed\n");
	pthread_attr_destroy(&attr);
#endif

	__builtin_trap();
}

Proc *proctab(int n) {
	Proc *p;
	lock(&procs.l);
	for (p = procs.head; p && n; n--) p = p->next;
	unlock(&procs.l);
	return p;
}

Proc *procpid(long pid) {
	Proc *p;
	lock(&procs.l);
	for (p = procs.head; p; p = p->next) if (p->pid == pid) break;
	unlock(&procs.l);
	return p;
}

void
oshostintr(Proc *p)
{
	//pthread_kill((pthread_t)p->sigid, SIGUSR1);
}

void
osblock(void)
{
	//Sem *sem;
	//sem = up->os;
	//pthread_mutex_lock(&sem->m);
	//while(sem->v == 0)
	//	pthread_cond_wait(&sem->c, &sem->m);
	//sem->v--;
	//pthread_mutex_unlock(&sem->m);
}

void
osready(Proc *p)
{
	//Sem *sem;
	//sem = p->os;
	//pthread_mutex_lock(&sem->m);
	//sem->v++;
	//pthread_cond_signal(&sem->c);
	//pthread_mutex_unlock(&sem->m);
}

void
oslopri(void)
{
  ;
}

#if 0
void
oslongjmp(void *regs, osjmpbuf env, int val)
{
	USED(regs);
	siglongjmp(env, val);
}

int gidnobody= -1, uidnobody= -1;

void
getnobody()
{
	struct passwd *pwd;

	if((pwd = getpwnam("nobody"))) {
		uidnobody = pwd->pw_uid;
		gidnobody = pwd->pw_gid;
	}
}
#endif

void
libinit(char *imod)
{
	struct passwd *pw;
	Proc *p;
	char	sys[64];

	//setsid();

	// setup personality
	//gethostname(sys, sizeof(sys));
	kstrdup(&ossysname, sys);
	//getnobody();

	//if(dflag == 0)
	//	termset();

	//setsigs();

	//if(pthread_key_create(&prdakey, NULL))
	//	print("key_create failed\n");

	p = newproc();
	if(pthread_setspecific(prdakey, p))
		panic("set specific thread data failed\n");

  pw = nil; //getpwuid(getuid());
	if(pw != nil)
		kstrdup(&eve, pw->pw_name);
	else
		print("cannot getpwuid\n");

  up->env->uid = 1; //getuid();
  up->env->gid = 1; //getgid();

	emuinit(imod);
	lock(&procs.l);
	if(procs.tail != nil) {
		p->prev = procs.tail;
		procs.tail->next = p;
	} else {
		procs.head = p;
		p->prev = nil;
	}
	procs.tail = p;
	unlock(&procs.l);

}

vlong
nsec(void)
{
	struct timespec t;
	if (!clock_gettime(0, &t)) return (vlong)t.tv_sec*1000000000L + t.tv_nsec;
	else return -1;
}

time_t
time(time_t *p)
{
	time_t t = (time_t)(nsec() / 1000000000);
	if (p) *p = t;
	return t;
}

int
_tas(int *l)
{
  ulong v;
  
  //while(!(mutexlock(&mulock)))
  //  pthread_yield();
  
  v = *l;
  if(v == 0)
    *l = 1;
  //mulock.key = 1;
  return v;
}

int ossetjmp() {
  return 0;
}

void
addprog(Proc *p)
{
  //Prog *n;
  //n = malloc(sizeof(Prog));
  //if(n == nil)
  //  panic("no memory");
  //p->prog = n;
  //n->osenv = p->env;
}

int
osmillisleep(ulong milsec)
{
	struct timespec time;
	time.tv_sec = milsec / 1000;
	time.tv_nsec = (milsec % 1000) * 1000000;
	//nanosleep(&time, nil);
	return -1; //0
}

void
osyield(void)
{
	//pthread_yield_np();
}

int cflag;
int keepbroken = 1;
int mutator = 0;
static size_t available = 0;
Bhdr*  ptr;

void *currun() {
  return nil;
}

/* the following is not needed when devfs available */
char  rootdir[MAXROOT] = "/"; //ROOT;
void
setid(char *name, int owner)
{
  if(!owner || iseve())
    kstrdup(&up->env->user, name);
}

void *pthread_specific_data;

void *pthread_getspecific(pthread_key_t key) {
  return pthread_specific_data;
}

int pthread_setspecific(pthread_key_t key, const void *data) {
  pthread_specific_data = data;
  return 0;
}

char* clipread(void) {
  return "empty clipboard";
}

int clipwrite(char* str) {
  return 0;
}

void
acquire(void)
{
}

void
release(void)
{
}
