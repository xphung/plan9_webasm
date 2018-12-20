#ifndef _PLAN9_SOURCE
#define _PLAN9_SOURCE

#define nil		((void*)0)
typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
typedef unsigned long	ulong;
typedef unsigned int	uint;
typedef signed char	schar;
typedef	long long	vlong;
typedef	unsigned long	long uvlong;
typedef unsigned long	long uintptr;
typedef unsigned long	usize;
typedef	uint		Rune;
typedef uintptr		jmp_buf[2];
typedef unsigned int	mpdigit;	/* for /sys/include/mp.h */
typedef unsigned char	u8int;
typedef unsigned short	u16int;
typedef unsigned int	u32int;
typedef unsigned long	long u64int;

typedef union FPdbleword FPdbleword;
union FPdbleword
{
	double	x;
	struct {	/* little endian */
		uint lo;
		uint hi;
	};
};

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef long ssize_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
#endif

#include <stdarg.h>
#define	USED(x)		if(x){}else{}
#define	SET(x)

#define abort	        kabort
#define atexit          atexits
#define atexitdont      atexitsdont
#define getenv		kgetenv
#define memccpy		kmemccpy
#define qsort           kqsort
#define rand		krand
#define srand		kseedrand
#define strtod		kstrtod
#define strtol		kstrtol
#define strtoul		kstrtoul
#define strtoll		kstrtoll
#define strtoull	kstrtoull
#define strchrnul	kstrchr
#define time(n)         ktime(n)
#define timezone        ktimezone

#define fabs		__builtin_fabs
#define fmod		kfmod
#define frexp		kfrexp
#define ldexp		kldexp
#define modf		kmodf

#endif
