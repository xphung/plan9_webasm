#ifndef _LIBC_ARCH_H
#define _LIBC_ARCH_H


#define signbitl	__builtin_signbitl
#define __fmodl		__builtin_fmodl
//#define __fpclassifyl(x) __builtin_fpclassifyl

// musl C lib stubs
extern const char *	__lctrans_impl();
extern char *		__gettextdomain();
extern void		__pthread_tsd_run_dtors();
extern void		__env_rm_add(char *old, char *new);
extern void		__funcs_on_quick_exit();
extern void		__funcs_on_exit();
extern void		__stdio_exit();
extern void		__fork_handler(int);
extern void		_noop_fn();
#define _init		_noop_fn
#define _fini		_noop_fn
#define _dlstart	_noop_fn
#define __init_tls	_noop_fn
#define	__init_array_start _noop_ptr
#define	__init_array_end   _noop_ptr
#define	__fini_array_start _noop_ptr
#define	__fini_array_end   _noop_ptr
//#define __block_all_sigs   _noop_fn

extern void		__unlist_locked_file(FILE *);
extern FILE *volatile	__stdin_used;
extern FILE *volatile	__stdout_used;
extern FILE *volatile	__stderr_used;
static inline void	__stdio_exit_needed(void) { __stdio_exit(); }

// dummy musl aio & pthread funcs
#define __do_cleanup_push dummy
#define __do_cleanup_pop dummy
static inline int	__aio_close(fd)  { return fd; }

// export musl weak alias funcs
#define __crypt_r	crypt_r
#define __isalnum_l	isalnum_l
#define __isalpha_l	isalpha_l
#define __isblank_l	isblank_l
#define __iscntrl_l	iscntrl_l
#define __isdigit_l	isdigit_l
#define __isgraph_l	isgraph_l
#define __islower_l	islower_l
#define __isprint_l	isprint_l
#define __ispunct_l	ispunct_l
#define __isspace_l	isspace_l
#define __isupper_l	isupper_l
#define __iswalnum_l	iswalnum_l
#define __iswalpha_l	iswalpha_l
#define __iswblank_l	iswblank_l
#define __iswcntrl_l	iswcntrl_l
#define __iswctype_l	iswctype_l
#define __wctype_l	wctype_l
#define __iswdigit_l	iswdigit_l
#define __iswgraph_l	iswgraph_l
#define __iswlower_l	iswlower_l
#define __iswprint_l	iswprint_l
#define __iswpunct_l	iswpunct_l
#define __iswspace_l	iswspace_l
#define __iswupper_l	iswupper_l
#define __iswxdigit_l	iswxdigit_l
#define __isxdigit_l	isxdigit_l
#define __tolower_l	tolower_l
#define __toupper_l	toupper_l
#define __towupper_l	towupper_l
#define __towlower_l	towlower_l
#define __wctrans_l	wctrans_l
#define __towctrans_l	towctrans_l
#define __init_ssp	dummy1
#define __environ	environ
//#define __environ	___environ
//#define __environ	_environ
#define libc_start_init	__libc_start_init
//#define static_init_tls __init_tls
//#define __getdents	getdents
//#define _init dummy
//#define __stack_chk_fail	__stack_chk_fail_local
#define __strerror_l	strerror_l
#define libc_exit_fini	__libc_exit_fini
//#define __progname	program_invocation_short_name
//#define __progname_full	program_invocation_name
//#define stub_dlsym	__dlsym
//#define static_dl_iterate_phdr	dl_iterate_phdr
//#define stub_dladdr	dladdr
//#define stub_invalid_handle	__dl_invalid_handle
//#define stub_dlopen	dlopen
//#define __tlsdesc_static	__tlsdesc_dynamic
//#define euidaccess	eaccess
//#define endutxent	endutent
//#define setutxent	setutent
//#define getutxent	getutent
//#define getutxid	getutid
//#define getutxline	getutline
//#define pututxline	pututline
//#define updwtmpx	updwtmp
#define __utmpname	utmpname
#define __utmpxname	utmpxname
#define _flush_cache	cacheflush
#define __cachectl	cachectl
#define __lsysinfo	sysinfo
#define __duplocale	duplocale
//#define freelocale	__freelocale
#define __nl_langinfo	nl_langinfo
#define __nl_langinfo_l	nl_langinfo_l
#define __newlocale	newlocale
#define __strcoll_l	strcoll_l
#define __strxfrm_l	strxfrm_l
#define __uselocale	uselocale
#define __wcscoll_l	wcscoll_l
#define __wcsxfrm_l	wcsxfrm_l
#define __simple_malloc	malloc
//#define __simple_malloc	__malloc0
#define __memalign	memalign
//#define exp10	pow10
//#define exp10f	pow10f
//#define exp10l	pow10l
#define __lgamma_r	lgamma_r
#define __lgammaf_r	lgammaf_r
#define __lgammal_r	lgammal_r
//#define remainder	drem
//#define remainderf	dremf
#define __signgam	signgam
#define __lgamma_r	lgamma_r
//#define basename	__xpg_basename
#define __optreset	optreset
//#define getopt	__posix_getopt
#define __ptsname_r	ptsname_r
#define __vsyslog	vsyslog
#define __madvise	madvise
//#define __mmap	mmap
//#define __mprotect	mprotect
//#define __mremap	mremap
//#define __munmap	munmap
#define __dn_comp	dn_comp
#define __dn_expand	dn_expand
//#define sethostent	setnetent
//#define gethostent	getnetent
//#define endhostent	endnetent
#define __inet_aton	inet_aton
#define __res_mkquery	res_mkquery
#define __res_query	res_query
//#define __res_query	res_search
#define __res_send	res_send
//#define setgrent	endgrent
//#define setpwent	endpwent
#define __execvpe	execvpe

#if 0
#define __vfork	vfork
#define __hcreate_r	hcreate_r
#define __hdestroy_r	hdestroy_r
#define __hsearch_r	hsearch_r
#define __sigaction	sigaction
#define signal	bsd_signal
#define signal	__sysv_signal
#endif

#define __futimesat	futimesat
#define __statfs	statfs
#define __fstatfs	fstatfs
#define __fdopen	fdopen
#define __fpurge	fpurge
#define __fseeko	fseeko
#define __ftello	ftello
//#define __stdio_exit	__stdio_exit_needed

#if 0
#define __fgetwc_unlocked	fgetwc_unlocked
#define __fgetwc_unlocked	getwc_unlocked
#define __fputwc_unlocked	fputwc_unlocked
#define __fputwc_unlocked	putwc_unlocked
#define clearerr	clearerr_unlocked
#define feof	feof_unlocked
#define feof	_IO_feof_unlocked
#define ferror	ferror_unlocked
#define ferror	_IO_ferror_unlocked
#define fflush	fflush_unlocked
#define fgets	fgets_unlocked
#define fgetws	fgetws_unlocked
#define fileno	fileno_unlocked
#define fputs	fputs_unlocked
#define fputws	fputws_unlocked
#define fread	fread_unlocked
#define fwrite	fwrite_unlocked
#define getc_unlocked	fgetc_unlocked
#define getc_unlocked	_IO_getc_unlocked
#define getwchar	getwchar_unlocked
#define putc_unlocked	fputc_unlocked
#define putc_unlocked	_IO_putc_unlocked
#define putwchar	putwchar_unlocked
#endif

//#define getdelim	__getdelim
//#define getc	_IO_getc
//#define putc	_IO_putc

#if 0
#define fscanf	__isoc99_fscanf
#define fwscanf,__isoc99_fwscanf
#define scanf,__isoc99_scanf
#define sscanf,__isoc99_sscanf
#define swscanf,__isoc99_swscanf
#define vfscanf,__isoc99_vfscanf
#define vfwscanf,__isoc99_vfwscanf
#define vscanf,__isoc99_vscanf
#define vsscanf,__isoc99_vsscanf
#define vswscanf,__isoc99_vswscanf
#define vwscanf,__isoc99_vwscanf
#define wscanf,__isoc99_wscanf
#endif

#if 0
#define strtof	strtof_l
#define strtod	strtod_l
#define strtold	strtold_l
#define strtof	__strtof_l
#define strtod	__strtod_l
#define strtold	__strtold_l
#define strtol	__strtol_internal
#define strtoul	__strtoul_internal
#define strtoll	__strtoll_internal
#define strtoull	__strtoull_internal
#define strtoimax	__strtoimax_internal
#define strtoumax	__strtoumax_internal
#endif

#define __memrchr	memrchr
#define __stpcpy	stpcpy
#define __stpncpy	stpncpy
#define __strcasecmp_l	strcasecmp_l
#define __strchrnul	strchrnul
#define __strdup	strdup
//#define strerror_r	__xpg_strerror_r
#define __strncasecmp_l	strncasecmp_l
#define __mkostemps	mkostemps
//#define __mkostemps	mkostemps64
//#define cfsetospeed	cfsetspeed
#define sccp __syscall_cp_c

#if 0
//#define __tls_get_addr	__tls_get_new
//#define __pthread_cond_timedwait	pthread_cond_timedwait
//#define __pthread_exit	pthread_exit
//#define __pthread_create	pthread_create
//#define __pthread_detach	pthread_detach
//#define __pthread_detach	thrd_detach
//#define __pthread_equal	pthread_equal
//#define __pthread_equal	thrd_equal
//#define __pthread_getspecific	pthread_getspecific
//#define __pthread_getspecific	tss_get
//#define __pthread_tryjoin_np	pthread_tryjoin_np
//#define __pthread_timedjoin_np	pthread_timedjoin_np
//#define __pthread_join	pthread_join
//#define __pthread_key_delete	pthread_key_delete
//#define __pthread_key_create	pthread_key_create
//#define __pthread_mutex_lock	pthread_mutex_lock
//#define __pthread_mutex_timedlock	pthread_mutex_timedlock
//#define __pthread_mutex_trylock	pthread_mutex_trylock
//#define __pthread_mutex_unlock	pthread_mutex_unlock
//#define __pthread_once	pthread_once
#define __pthread_self_internal	pthread_self
#define __pthread_setcancelstate	pthread_setcancelstate
//#define __pthread_testcancel	pthread_testcancel
#endif

#define __timezone	timezone
#define __daylight	daylight
#define __tzname	tzname
#define __tzset		tzset
#define __clock_gettime	clock_gettime
#define __gmtime_r	gmtime_r
#define __localtime_r	localtime_r
#define __strftime_l	strftime_l
#define __wcsftime_l	wcsftime_l
#define __dup3		dup3
#define __lsysinfo	sysinfo

#endif


