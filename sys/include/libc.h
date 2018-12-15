/*
 *  Inferno kernel and musl each have their own versions of C string and stdlib functions
 */
#ifndef _PLAN9_SOURCE
/*
 *  If _PLAN9_SOURCE is not set then source files are compiled using musl POSIX compliant string functions
 *  In this case, libc.h is an internal musl include file (completely unrelated to Plan9 libc.h)
 */
#include <libc_musl.h>
#else
/*
 *  If _PLAN9_SOURCE is set then source files are compiled to use Inferno 'k' versions of stdlib functions
 *  - Inferno stdlib strtoxx functions do not set errno
 *  - Inferno string functions do not have POSIX compliant const/restrict qualifiers
 */
#ifndef _PLAN9_LIBC_H
#define _PLAN9_LIBC_H
#define _STRING_H	// prevent musl string.h definitions
#define _STDLIB_H	// prevent musl stdlib.h definitions

#define	nelem(x)	(sizeof(x)/sizeof((x)[0]))
#define	offsetof(s, m)	(ulong)(&(((s*)0)->m))
#define	assert(x)	if(x){}else _assert("x")
#define intern		static inline

/*
 * mem routines
 */
extern	void*	memset(void* p, int c, size_t n); //{ for(char *r = p; n; n--, r++) *r = c; return p; }
extern	void*	memcpy(void* p, const void* q, size_t n);
extern  void*	memccpy(void*p, const void* q, int c, size_t n);
intern  void*	memchr(void* p, int c, size_t n) { char *sp = p; for(c = (char)c; n > 0; n--) if(*sp++ == c) return sp-1; return 0; }
intern  int	memcmp(const void* p, const void* q, size_t n) { uchar *s=p, *t=q; for (;n && *s==*t; s++,t++) n--; return n ? *s-*t : 0; }
intern  void*	memmove(void*p, const void* q, size_t n) { return memcpy(p, q, n); }

/*
 * string routines
 */
extern	void*	malloc(size_t size);
extern	char*	strchrnul(const char *s, int c);
intern	char*	strchr(char* s, int c) { char *r = strchrnul(s, c); return *(char *)r == (char)c ? r : 0; }
intern	char*	strrchr(char* s, int c) { const char *p, *f = nil; while ((p = strchr(s, c))) { f = p; s = p + 1; } return (char *) f; }
intern	size_t	strlen(char* s) { return (size_t) (strchrnul(s, 0) - s); }
intern	int	strncmp(char* l, char* r, size_t n) { if (!n--) return 0; for (; *l && *r && n && *l == *r ; l++, r++) n--; return *l - *r; }
intern	int	strcmp(char *p, char *q) { return strncmp(p,q, 0xFFFF); }
intern	char*	strncpy(char* p, char* q, long n) { q = memccpy(p, q, 0, n); if (q && q<p+n) memset(q, 0, p+n-q); return p; }
intern	char*	strcpy(char *p, char *q) { memccpy(p,q, 0, 0x7FFFFFFF); return p; }
intern	char*	strcat(char *p, char *q) { strcpy(strchrnul(p, 0), q); return p; }
intern	char*	strdup(const char *q) { char *p = malloc(strlen(q)+1); return p ? strcpy(p, q) : 0; }
intern	char*	strstr(char *r, char *s) { char f=s[0]; if(!f)return r; for(char *p=strchr(r,f); ; p=strchr(p+1,f))if(!p||!strcmp(p,s))return p; }
extern	char*	strecpy(char*, char*, char*);
extern	int	cistrncmp(char*, char*, int);
extern	int	cistrcmp(char*, char*);
extern	char*	cistrstr(char*, char*);
extern	int	tokenize(char*, char**, int);

enum
{
	UTFmax		= 4,		/* maximum bytes per rune */
	Runesync	= 0x80,		/* cannot represent part of a UTF sequence (<) */
	Runeself	= 0x80,		/* rune and UTF sequences are the same (<) */
	Runeerror	= 0xFFFD,	/* decoding error in UTF */
	Runemax		= 0x10FFFF,	/* 21-bit rune */
	Runemask	= 0x1FFFFF,	/* bits used by runes (see grep) */
};

/*
 * rune routines
 */
extern	int	runetochar(char*, Rune*);
extern	int	chartorune(Rune*, char*);
extern	int	runelen(long);
extern	int	runenlen(Rune*, int);
extern	int	fullrune(char*, int);
extern	int	utflen(char*);
extern	int	utfnlen(char*, long);
extern	char*	utfrune(char*, long);
extern	char*	utfrrune(char*, long);
extern	char*	utfutf(char*, char*);
extern	char*	utfecpy(char*, char*, char*);

#ifndef EMU
extern	Rune*	runestrcat(Rune*, Rune*);
extern	Rune*	runestrchr(Rune*, Rune);
extern	int	runestrcmp(Rune*, Rune*);
extern	Rune*	runestrcpy(Rune*, Rune*);
extern	Rune*	runestrncpy(Rune*, Rune*, long);
extern	Rune*	runestrecpy(Rune*, Rune*, Rune*);
extern	Rune*	runestrdup(Rune*);
extern	Rune*	runestrncat(Rune*, Rune*, long);
extern	int	runestrncmp(Rune*, Rune*, long);
extern	Rune*	runestrrchr(Rune*, Rune);
extern	long	runestrlen(Rune*);
extern	Rune*	runestrstr(Rune*, Rune*);

extern	Rune	tolowerrune(Rune);
extern	Rune	totitlerune(Rune);
extern	Rune	toupperrune(Rune);
extern	Rune	tobaserune(Rune);
extern	int	isalpharune(Rune);
extern	int	isbaserune(Rune);
extern	int	isdigitrune(Rune);
extern	int	islowerrune(Rune);
extern	int	isspacerune(Rune);
extern	int	istitlerune(Rune);
extern	int	isupperrune(Rune);
#endif

/*
 * malloc
 */
extern	void*	malloc(size_t);
extern	void*	mallocz(ulong, int);
extern	void	free(void*);
extern	ulong	msize(void*);
extern	void*	calloc(size_t, size_t);
extern	void*	realloc(void*, size_t);
extern	void	setmalloctag(void*, ulong);
extern	void	setrealloctag(void*, ulong);
extern	ulong	getmalloctag(void*);
extern	ulong	getrealloctag(void*);
extern	void*	malloctopoolblock(void*);

/*
 * print routines
 */
typedef struct Fmt	Fmt;
struct Fmt{
	uchar	runes;			/* output buffer is runes or chars? */
	void	*start;			/* of buffer */
	void	*to;			/* current place in the buffer */
	void	*stop;			/* end of the buffer; overwritten if flush fails */
	int	(*flush)(Fmt *);	/* called when to == stop */
	void	*farg;			/* to make flush a closure */
	int	nfmt;			/* num chars formatted so far */
	va_list	args;			/* args passed to dofmt */
	int	r;			/* % format Rune */
	int	width;
	int	prec;
	ulong	flags;
};

enum{
	FmtWidth	= 1,
	FmtLeft		= FmtWidth << 1,
	FmtPrec		= FmtLeft << 1,
	FmtSharp	= FmtPrec << 1,
	FmtSpace	= FmtSharp << 1,
	FmtSign		= FmtSpace << 1,
	FmtZero		= FmtSign << 1,
	FmtUnsigned	= FmtZero << 1,
	FmtShort	= FmtUnsigned << 1,
	FmtLong		= FmtShort << 1,
	FmtVLong	= FmtLong << 1,
	FmtComma	= FmtVLong << 1,
	FmtByte		= FmtComma << 1,

	FmtFlag		= FmtByte << 1
};

extern	int	print(char*, ...);
extern	char*	seprint(char*, char*, char*, ...);
extern	char*	vseprint(char*, char*, char*, va_list);
extern	int	snprint(char*, int, char*, ...);
extern	int	vsnprint(char*, int, char*, va_list);
extern	char*	smprint(char*, ...);
extern	char*	vsmprint(char*, va_list);
extern	int	sprint(char*, char*, ...);
extern	int	fprint(int, char*, ...);
extern	int	vfprint(int, char*, va_list);

#ifndef EMU
extern	int	runesprint(Rune*, char*, ...);
extern	int	runesnprint(Rune*, int, char*, ...);
extern	int	runevsnprint(Rune*, int, char*, va_list);
extern	Rune*	runeseprint(Rune*, Rune*, char*, ...);
extern	Rune*	runevseprint(Rune*, Rune*, char*, va_list);
extern	Rune*	runesmprint(char*, ...);
extern	Rune*	runevsmprint(char*, va_list);
#endif

extern	int	fmtfdinit(Fmt*, int, char*, int);
extern	int	fmtfdflush(Fmt*);
extern	int	fmtstrinit(Fmt*);
extern	char*	fmtstrflush(Fmt*);
extern	int	runefmtstrinit(Fmt*);
extern	Rune*	runefmtstrflush(Fmt*);

#pragma	varargck	argpos	fmtprint	2
#pragma	varargck	argpos	fprint		2
#pragma	varargck	argpos	print		1
#pragma	varargck	argpos	runeseprint	3
#pragma	varargck	argpos	runesmprint	1
#pragma	varargck	argpos	runesnprint	3
#pragma	varargck	argpos	runesprint	2
#pragma	varargck	argpos	seprint		3
#pragma	varargck	argpos	smprint		1
#pragma	varargck	argpos	snprint		3
#pragma	varargck	argpos	sprint		2

#pragma	varargck	type	"lld"	vlong
#pragma	varargck	type	"llo"	vlong
#pragma	varargck	type	"llx"	vlong
#pragma	varargck	type	"llb"	vlong
#pragma	varargck	type	"lld"	uvlong
#pragma	varargck	type	"llo"	uvlong
#pragma	varargck	type	"llx"	uvlong
#pragma	varargck	type	"llb"	uvlong
#pragma	varargck	type	"ld"	long
#pragma	varargck	type	"lo"	long
#pragma	varargck	type	"lx"	long
#pragma	varargck	type	"lb"	long
#pragma	varargck	type	"ld"	ulong
#pragma	varargck	type	"lo"	ulong
#pragma	varargck	type	"lx"	ulong
#pragma	varargck	type	"lb"	ulong
#pragma	varargck	type	"d"	int
#pragma	varargck	type	"o"	int
#pragma	varargck	type	"x"	int
#pragma	varargck	type	"c"	int
#pragma	varargck	type	"C"	int
#pragma	varargck	type	"b"	int
#pragma	varargck	type	"d"	uint
#pragma	varargck	type	"x"	uint
#pragma	varargck	type	"c"	uint
#pragma	varargck	type	"C"	uint
#pragma	varargck	type	"b"	uint
#pragma	varargck	type	"f"	double
#pragma	varargck	type	"e"	double
#pragma	varargck	type	"g"	double
#pragma	varargck	type	"s"	char*
#pragma	varargck	type	"q"	char*
#pragma	varargck	type	"S"	Rune*
#pragma	varargck	type	"Q"	Rune*
#pragma	varargck	type	"r"	void
#pragma	varargck	type	"%"	void
#pragma	varargck	type	"n"	int*
#pragma	varargck	type	"p"	uintptr
#pragma	varargck	type	"p"	void*
#pragma	varargck	flag	','
#pragma	varargck	flag	' '
#pragma	varargck	flag	'h'
#pragma varargck	type	"<"	void*
#pragma varargck	type	"["	void*
#pragma varargck	type	"H"	void*
#pragma varargck	type	"lH"	void*

extern	int	fmtinstall(int, int (*)(Fmt*));
extern	int	dofmt(Fmt*, char*);
extern	int	dorfmt(Fmt*, Rune*);
extern	int	fmtprint(Fmt*, char*, ...);
extern	int	fmtvprint(Fmt*, char*, va_list);
extern	int	fmtrune(Fmt*, int);
extern	int	fmtstrcpy(Fmt*, char*);
extern	int	fmtrunestrcpy(Fmt*, Rune*);
/*
 * error string for %r
 * supplied on per os basis, not part of fmt library
 */
extern	int	errfmt(Fmt *f);

/*
 * quoted strings
 */
extern	char	*unquotestrdup(char*);
extern	Rune	*unquoterunestrdup(Rune*);
extern	char	*quotestrdup(char*);
extern	Rune	*quoterunestrdup(Rune*);
extern	int	quotestrfmt(Fmt*);
extern	int	quoterunestrfmt(Fmt*);
extern	void	quotefmtinstall(void);
extern	int	(*doquote)(int);

/*
 * random number
 */
extern	void	srand(long);
extern	int	rand(void);
extern	int	nrand(int);
#ifndef EMU
extern	long	lrand(void);
extern	long	lnrand(long);
extern	double	frand(void);
#endif
extern	ulong	truerand(void);			/* uses /dev/random */
extern	ulong	ntruerand(ulong);		/* uses /dev/random */

/*
 * math
 */
extern	double	NaN(void);
extern	double	Inf(int);
extern	int	isNaN(double);
extern	int	isInf(double, int);
#ifndef EMU
extern	ulong	umuldiv(ulong, ulong, ulong);
extern	long	muldiv(long, long, long);

extern	double	pow(double, double);
extern	double	atan2(double, double);
extern	double	fabs(double);
extern	double	atan(double);
extern	double	log(double);
extern	double	log10(double);
extern	double	exp(double);
extern	double	floor(double);
extern	double	ceil(double);
extern	double	hypot(double, double);
extern	double	sin(double);
extern	double	cos(double);
extern	double	tan(double);
extern	double	asin(double);
extern	double	acos(double);
extern	double	sinh(double);
extern	double	cosh(double);
extern	double	tanh(double);
extern	double	sqrt(double);
extern	double	fmod(double, double);
#endif

#define	HUGE	3.4028234e38
#define PIO2	1.570796326794896619231e0
#define PI	(PIO2+PIO2)

#define fabs(x)	    __builtin_fabs((x))
#define NaN()	    __builtin_nan("")
#define Inf(s)	    __builtin_copysign(__builtin_inf(), (s))
#define isNaN(x)    __builtin_isnan((x))
#define isInf(x,s) (__builtin_isinf((x)) ? ((x) > 0 ? (s) >= 0 : (s) <= 0) : 0)

/*
 * time-of-day
 */
typedef struct Tm Tm;
struct Tm {
	int	sec;
	int	min;
	int	hour;
	int	mday;
	int	mon;
	int	year;
	int	wday;
	int	yday;
	char	zone[4];
	int	tzoff;
};

extern	Tm*	gmtime(long);
extern	Tm*	localtime(long);
extern	char*	asctime(Tm*);
extern	char*	ctime(long);
extern	double	cputime(void);
extern	long	times(long*);
extern	long	tm2sec(Tm*);
extern	vlong	nsec(void);

/*
 * clock_gettime extension
 */
#ifndef __DEFINED_struct_timespec)
#define __DEFINED_struct_timespec
struct timespec { time_t tv_sec; long tv_nsec; };
#endif
int clock_gettime(int, struct timespec *);

/*
 * one-of-a-kind
 */
extern	void	 _assert(char*);
extern	int	abs(int);
extern  int     atexit(void(*)(void));
extern  void    atexitdont(void(*)(void));
extern  int     atnotify(int(*)(void*, char*), int);

extern  vlong   kstrtoll(const char*, char**, int);
intern	int	atoi(char *x)	{ return (int) kstrtoll(x, nil, 10); }
intern	long	atol(char *x)	{ return (long) kstrtoll(x, nil, 10); }
intern	vlong	atoll(char *x)	{ return kstrtoll(x, nil, 10); }
extern	double  charstod(int(*)(void*), void*);
extern	char*   cleanname(char*);

extern	int	dec64(uchar*, int, char*, int);
extern	int	enc64(char*, int, uchar*, int);
extern	int	dec32(uchar*, int, char*, int);
extern	int	enc32(char*, int, uchar*, int);
extern	int	dec16(uchar*, int, char*, int);
extern	int	enc16(char*, int, uchar*, int);
extern	void	_Noreturn exits(char*);
extern	double  frexp(double, int*);
extern	ulong	getcallerpc(void*);
extern	char*	getenv(char*);
extern	int	getfields(char*, char**, int, int, char*);
extern	char*	getuser(void);
extern	char*	getwd(char*, int);
extern	double  ipow10(int);
extern	double  ldexp(double, int);
extern	double  modf(double, double*);

extern	void    perror(const char*);
extern	double  pow10(int);
extern  void    qsort(void*, long, long, int (*)(void*, void*));
extern  double  strtod(char*, char**);
extern  long    strtol(char*, char**, int);
extern  ulong   strtoul(char*, char**, int);
extern  vlong   strtoll(const char*, char**, int);
extern  uvlong  strtoull(char*, char**, int);
extern	void    sysfatal(char*, ...);
extern  long    time(long*);
extern  int     tolower(int);
extern  int     toupper(int);
#include <ctype.h>

/*
 *  synchronization
 */
#ifndef EMU
typedef struct	Proc Proc;
#endif

typedef struct	Lock Lock;
struct Lock {
	int	val;
	int	pid;
};

extern int	_tas(int*);

extern	void	lock(Lock*);
extern	void	unlock(Lock*);
extern	int	canlock(Lock*);

typedef struct	QLock QLock;
struct QLock
{
	Lock	use;			/* to access Qlock structure */
	Proc	*head;			/* next process waiting for object */
	Proc	*tail;			/* last process waiting for object */
	int	locked;			/* flag */
};

extern	void	qlock(QLock*);
extern	void	qunlock(QLock*);
extern	int	canqlock(QLock*);
extern	void	_qlockinit(ulong (*)(ulong, ulong));	/* called only by the thread library */

typedef
struct RWLock
{
	Lock	l;			/* Lock modify lock */
	QLock	x;			/* Mutual exclusion lock */
	QLock	k;			/* Lock for waiting writers */
	int	readers;		/* Count of readers in lock */
} RWLock;

extern	int	canrlock(RWLock*);
extern	int	canwlock(RWLock*);
extern	void	rlock(RWLock*);
extern	void	runlock(RWLock*);
extern	void	wlock(RWLock*);
extern	void	wunlock(RWLock*);

/*
 * network dialing
 */
#define NETPATHLEN 40

/*
 * system calls
 *
 */
#define getpid		kgetpid
#define getwd		kgetwd

#define	STATMAX	65535U	/* max length of machine-independent stat structure */
#define	DIRMAX	(sizeof(Dir)+STATMAX)	/* max length of Dir structure */
#define	ERRMAX	128	/* max length of error string */

#define	MORDER	0x0003	/* mask for bits defining order of mounting */
#define	MREPL	0x0000	/* mount replaces object */
#define	MBEFORE	0x0001	/* mount goes before others in union directory */
#define	MAFTER	0x0002	/* mount goes after others in union directory */
#define	MCREATE	0x0004	/* permit creation in mounted directory */
#define	MCACHE	0x0010	/* cache some data */
#define	MMASK	0x0017	/* all bits on */

#define	OREAD	0	/* open for read */
#define	OWRITE	1	/* write */
#define	ORDWR	2	/* read and write */
#define	OEXEC	3	/* execute, == read but check execute permission */
#define	OTRUNC	16	/* or'ed in (except for exec), truncate file first */
#define	OCEXEC	32	/* or'ed in, close on exec */
#define	ORCLOSE	64	/* or'ed in, remove on close */
#define	OEXCL	0x1000	/* or'ed in, exclusive use (create only) */

#define	AEXIST	0	/* accessible: exists */
#define	AEXEC	1	/* execute access */
#define	AWRITE	2	/* write access */
#define	AREAD	4	/* read access */

/* bits in Qid.type */
#define QTDIR		0x80		/* type bit for directories */
#define QTAPPEND	0x40		/* type bit for append only files */
#define QTEXCL		0x20		/* type bit for exclusive use files */
#define QTMOUNT		0x10		/* type bit for mounted channel */
#define QTAUTH		0x08		/* type bit for authentication file */
#define QTFILE		0x00		/* plain file */

/* bits in Dir.mode */
#define DMDIR		0x80000000	/* mode bit for directories */
#define DMAPPEND	0x40000000	/* mode bit for append only files */
#define DMEXCL		0x20000000	/* mode bit for exclusive use files */
#define DMMOUNT		0x10000000	/* mode bit for mounted channel */
#define DMAUTH		0x08000000	/* mode bit for authentication file */
#define DMTMP		0x04000000	/* mode bit for non-backed-up files */
#define DMREAD		0x4		/* mode bit for read permission */
#define DMWRITE		0x2		/* mode bit for write permission */
#define DMEXEC		0x1		/* mode bit for execute permission */

/* rfork */
enum
{
	RFNAMEG		= (1<<0),
	RFENVG		= (1<<1),
	RFFDG		= (1<<2),
	RFNOTEG		= (1<<3),
	RFPROC		= (1<<4),
	RFMEM		= (1<<5),
	RFNOWAIT	= (1<<6),
	RFCNAMEG	= (1<<10),
	RFCENVG		= (1<<11),
	RFCFDG		= (1<<12),
	RFREND		= (1<<13),
	RFNOMNT		= (1<<14)
};

typedef
struct Qid
{
	uvlong	path;
	ulong	vers;
	uchar	type;
} Qid;

typedef
struct Dir {
	/* system-modified data */
	ushort	type;	/* server type */
	uint	dev;	/* server subtype */
	/* file data */
	Qid	qid;	/* unique id from server */
	ulong	mode;	/* permissions */
	ulong	atime;	/* last read time */
	ulong	mtime;	/* last write time */
	vlong	length;	/* file length */
	char	*name;	/* last element of path */
	char	*uid;	/* owner name */
	char	*gid;	/* group name */
	char	*muid;	/* last modifier name */
} Dir;

typedef
struct Waitmsg
{
	int pid;	/* of loved one */
	ulong time[3];	/* of loved one & descendants */
	char	*msg;
} Waitmsg;

#define bind	_BIND
#define chdir	_CHDIR
#define close	_CLOSE
#define create	_CREATE
#define dup	_DUP
#define fstat	_FSTAT
#define fwstat	_FWSTAT
#define mount	_MOUNT
#define open	_OPEN
#define pipe	_PIPE
#define pread	_PREAD
#define pwrite	_PWRITE
#define read	_READ
#define	remove	_REMOVE
#define rfork	_RFORK
#define seek	_SEEK
#define stat	_STAT
#define write	_WRITE
#define wstat	_WSTAT

extern	void	 _exits(char*);

extern  void	abort(void);
extern	int	bind(const char*, const char*, int);
extern	int	chdir(const char*);
extern  int	close(int);
extern  int	create(const char*, int, ulong);
extern	int	dup(int, int);
extern	int	errstr(char*, uint);
extern	int	rfork(int);

extern	int	fstat(int, void*, int);
extern	int	fwstat(int, void*, int);
extern	int	fversion(int, int, char*, int);
extern	int	mount(int, int, const char*, int, const char*);
extern	int	noted(int);
extern	int	notify(void(*)(void*, char*));
extern  int	open(const char*, int);
extern	int	fd2path(int, const char*, int);
extern	int	pipe(int*);
extern	long	pread(int, void*, long, vlong);
extern	long	pwrite(int, const void*, long, vlong);
extern	long	read(int, void*, long);
extern	long	readn(int, void*, long);
extern	int	remove(const char*);
extern  void*	sbrk(long);
extern	vlong	seek(int, vlong, int);
extern	int	segflush(void*, ulong);
extern	int	sleep(long);
extern	int	stat(const char*, void*, int);
extern  long	write(int, const void*, long);
extern	int	wstat(const char*, void*, int);
extern	void*	rendezvous(void*, void*);

extern	Dir*	dirstat(char*);
extern	Dir*	dirfstat(int);
extern	int	dirwstat(char*, Dir*);
extern	int	dirfwstat(int, Dir*);
extern	long	dirread(int, Dir**);
extern	void	nulldir(Dir*);
extern	long	dirreadall(int, Dir**);
extern	int	getpid(void);
extern	int	getppid(void);
extern	void	rerrstr(char*, uint);
extern	char*	sysname(void);
extern	void	werrstr(char*, ...);

extern char *argv0;
#define	ARGBEGIN	for((argv0||(argv0=*argv)),argv++,argc--;\
			    argv[0] && argv[0][0]=='-' && argv[0][1];\
			    argc--, argv++) {\
				char *_args, *_argt;\
				Rune _argc;\
				_args = &argv[0][1];\
				if(_args[0]=='-' && _args[1]==0){\
					argc--; argv++; break;\
				}\
				_argc = 0;\
				while(*_args && (_args += chartorune(&_argc, _args)))\
				switch(_argc)
#define	ARGEND		SET(_argt);USED(_argt);USED(_argc); USED(_args);}USED(argv); USED(argc);
#define	ARGF()		(_argt=_args, _args="",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): 0))
#define	EARGF(x)	(_argt=_args, _args="",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): ((x), abort(), (char*)0)))

#define	ARGC()		_argc

/*
 *	Extensions for Inferno to basic libc.h
 */

#define	setbinmode()
#undef  intern
#endif
#endif
