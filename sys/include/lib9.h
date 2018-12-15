#include <u.h>

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);
extern void _Noreturn _exit(int);
extern void _Noreturn exit(int);

#include <libc.h>
