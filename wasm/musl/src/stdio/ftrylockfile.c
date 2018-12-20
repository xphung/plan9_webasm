#include "stdio_impl.h"
#include "pthread_impl.h"
#include <limits.h>

#define MAYBE_WAITERS 0x40000000

void __do_orphaned_stdio_locks()
{
	return; // atomics not avail in wasm
}

void __unlist_locked_file(FILE *f)
{
	if (f->lockcount) {
		if (f->next_locked) f->next_locked->prev_locked = f->prev_locked;
		if (f->prev_locked) f->prev_locked->next_locked = f->next_locked;
		else __pthread_self()->stdio_locks = f->next_locked;
	}
}

void __register_locked_file(FILE *f, pthread_t self)
{
	f->lockcount = 1;
	f->prev_locked = 0;
	f->next_locked = self->stdio_locks;
	if (f->next_locked) f->next_locked->prev_locked = f;
	self->stdio_locks = f;
}

int ftrylockfile(FILE *f)
{
	return 0; // atomics not avail in wasm
}
