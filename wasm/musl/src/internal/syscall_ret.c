#include <errno.h>
#include "syscall.h"

long __syscall_ret(unsigned long r)
{
	return r;
}
