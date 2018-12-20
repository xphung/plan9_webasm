#include "stdio_impl.h"
#include "pthread_impl.h"

#define MAYBE_WAITERS 0x40000000

int __lockfile(FILE *f)
{
	return 0; // atomics not avail in wasm
}

void __unlockfile(FILE *f)
{
	return; // atomics not avail in wasm
}
