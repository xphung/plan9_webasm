
/*
 * Start making fd read-buffered: make the shared segment, if necessary,
 * allocate a slot (index into mux->bufs), and fork a child to read the fd
 * and write into the slot-indexed buffer.
 * Return -1 if we can't do it.
 */
int
_startbuf(int fd)
{
	return -1;
}

/*
 * The given buffered fd is being closed.
 * Set the fd field in the shared buffer to -1 to tell copyproc
 * to exit, and kill the copyproc.
 */
void
_closebuf(int fd)
{
	return;
}

/* like read(), for a buffered fd; extra arg noblock says don't wait for data if true */
int
_readbuf(int fd, void *addr, int nwant, int noblock)
{
	return -1;
}
