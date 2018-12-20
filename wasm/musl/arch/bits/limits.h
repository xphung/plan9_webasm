#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define LONG_BIT 32
#endif

#define LONG_MAX  0x7fffffffL
#define LLONG_MAX  0x7fffffffffffffffLL

/*
 * APE limits.h
 */
#define	CHILD_MAX	75
#define	OPEN_MAX	96
#define	LINK_MAX	1
#define	MAX_CANON	1023
#define	MAX_INPUT	1023
#if 0	/* defined in musl limit.h */
#define	ARG_MAX		16384
#define	PATH_MAX	1023
#define	NAME_MAX	PATH_MAX
#define	NGROUPS_MAX	32
#define	PIPE_BUF	8192
#endif
