/*
 * system- and machine-specific declarations for emu:
 * floating-point save and restore, signal handling primitive, and
 * implementation of the current-process variable `up'.
 */
extern	Proc*		getup();
#define	up		(getup())

extern	void		(*coherence)(void);
extern	char*		clipread(void);
extern	int		clipwrite(char*);

extern	int		main(int argc, char *argv[]);
extern	void _Noreturn	sysfault(char *, void *);
extern	int     	kprint(int fd, const char *fmt, ...);
#define print(...)      kprint(1, __VA_ARGS__)
extern	void    	kabort(void);
extern	void    	kassert(char *s);
#define _assert(x)      kassert(x)
#define smalloc(x)	vsmalloc(x, __func__, "")
#define kmalloc(x)	vkmalloc(x, __func__, "")
static	inline long	osmillisec(void) { return nsec()/1000000; }
extern	ssize_t		tty_read(int fd, long buffer, size_t n);
extern	ssize_t		tty_write(int fd, long buffer, size_t n);

typedef	struct	FPU	FPU;

/*
 * This structure must agree with FPsave and FPrestore asm routines
 */
struct FPU
{
	int dummy[1];	/* XXX what goes here? */
};

typedef jmp_buf osjmpbuf;
extern	int     ossetjmp(jmp_buf);

