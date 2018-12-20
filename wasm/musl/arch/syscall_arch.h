#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#define __SYSCALL_LL_E(x) (x)
#define __SYSCALL_LL_O(x) (x)

typedef	long		syscall_arg_t;
typedef	unsigned long	ulong;
typedef	long long	vlong;
//typedef long sys_off_t;
//typedef unsigned long sys_size_t;

extern	long		js_call0(int);
extern	long		js_call1(int, long);
extern	long		js_call2(int, long, long);
extern	long		js_call3(int, long, long, long);
extern	long		js_call4(int, long, long, long, long);
extern	long		js_call5(int, long, long, long, long, long);
extern	long		js_call6(int, long, long, long, long, long, long);

static inline long __syscall0(int n)
{
	return js_call0(n);
}

static inline long __syscall1(int n, long a1)
{
	if (n == SYS_close) return close((int)a1);
	else if (n == SYS_chdir) return chdir((char *)a1);
	else if (n == SYS_dup) return dup((int)a1);
	else if (n == SYS_unlink) return unlink((char *)a1);
	else if (n == SYS_rmdir) return rmdir((char *)a1);
	else if (n == SYS_exit_group) _exit((int)a1);
	else return js_call1(n, (int)a1);
}

static inline long __syscall2(int n, long a1, long a2)
{
	if (n == SYS_clock_gettime) return clock_gettime(a1, (void *)a2);
	else if (n == SYS_creat) return creat((int)a1, (int)a2);
	else if (n == SYS_dup2) return dup2((int)a1, (int)a2);
	else if (n == SYS_stat) return stat((char *)a1, (void *)a2);
	else if (n == SYS_lstat) return stat((char *)a1, (void *)a2);
	else if (n == SYS_rename) return rename((char *)a1, (char *)a2);
	else return js_call2(n, (int)a1, (int)a2);
}

static inline long __syscall3(int n, long a1, long a2, long a3)
{
	if (n == SYS_open) return open((char *)a1, (long) a2);
	else if (n == SYS_lseek) return lseek((int)a1, a2, (int)a3);
	else if (n == SYS_fcntl) return fcntl((int)a1, (unsigned int)a2, (void *)a3);
	else if (n == SYS_ioctl) return 0;  //js_ioctl((int) a1, (unsigned int)a2, (sys_off_t)a3);
	else if (n == SYS_read) return read((int)a1, (void *)a2, a3);
	else if (n == SYS_readv)
	{
		struct iovec *iov = (struct iovec *) a2;
		struct iovec io = iov[0];
		return read((int)a1, io.iov_base, io.iov_len);
	}
	else if (n == SYS_write) return write((int)a1, (void *)a2, a3);
	else if (n == SYS_writev)
	{
		struct iovec *iov = (struct iovec *) a2;
		struct iovec io = iov[0];
		return write((int)a1, io.iov_base, io.iov_len);
	}
  	else return js_call3(n, a1, a2, a3);
}

static inline long __syscall4(int n, long a1, long a2, long a3, long a4)
{
	return js_call4(n, a1, a2, a3, a4);
}

static inline long __syscall5(int n, long a1, long a2, long a3, long a4, long a5)
{
	return js_call5(n, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5);
}

static inline long __syscall6(int n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	return js_call6((int)n, a1, a2, a3, a4, a5, a6);
}

#include <stdarg.h>
static inline long __syscall(long n, ...)
{
	va_list va;
	va_start(va, n);
	long a = va_arg(va, long);
	long b = va_arg(va, long);
	long c = va_arg(va, long);
	long d = va_arg(va, long);
	long e = va_arg(va, long);
	long f = va_arg(va, long);
	va_end(va);
	return __syscall6(n, a, b, c, d, e, f);
}
