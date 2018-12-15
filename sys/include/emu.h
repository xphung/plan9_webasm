/*
 * system- and machine-specific declarations for emu:
 * floating-point save and restore, signal handling primitive, and
 * implementation of the current-process variable `up'.
 */

extern	Proc*	getup();
#define	up	(getup())

typedef	struct	FPU	FPU;

/*
 * This structure must agree with FPsave and FPrestore asm routines
 */
struct FPU
{
	int dummy[1];	/* XXX what goes here? */
};

typedef jmp_buf osjmpbuf;
int     ossetjmp(jmp_buf);
int	main(int argc, char *argv[]);
void    _Noreturn sysfault(char *, void *);
int     kprint(int fd, const char *fmt, ...);
//char   *kgetcwd(char *buffer, size_t n);
void    kassert(char *s);
void    kabort(void);

#define print(...)      kprint(1, __VA_ARGS__)
#define _assert(x)      kassert(x)
static	inline long	osmillisec(void) { return nsec()/1000000; }
extern	ssize_t		cons_read(int fd, long buffer, size_t n);
extern	ssize_t		cons_write(int fd, long buffer, size_t n);
#define smalloc(x)	vsmalloc(x, __func__, "")
#define kmalloc(x)	vkmalloc(x, __func__, "")

