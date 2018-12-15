#include "dat.h"
#include "fns.h"
#include "kernel.h"

static int not_implemented() {
  return -1;
}

int _AWAIT(char* s, int n)
{
  return not_implemented();
}

int _ALARM(unsigned long n)
{
  return not_implemented();
}

long _BRK_(long n)
{
  return not_implemented();
}

int _BIND(const char* path1, const char* path2, int flags)
{
  return kbind((char *)path1, (char *)path2, flags);
}

int _CHDIR(const char* path)
{
  return kchdir((char *)path);
}

int _CLOSE(int fd)
{
  return kclose(fd);
}

int _CREATE(const char* path, int mode, unsigned long perm)
{
  return kcreate(path, mode, perm);
}

int _DUP(int old, int new)
{
  return kdup(old, new);
}

int _ERRSTR(char* s, unsigned int n)
{
  return not_implemented();
}

int errstr(char *s, unsigned int n)
{
  return _ERRSTR(s, n);
}

int _EXEC(char* cmd, char* args[])
{
  return -1;
}

void _EXITS(char *s)
{
  if (s) {
    kprint(1, "\nEXITS: %s\n", s);
    cleanexit(1);
  } else cleanexit(0);
}

void _exits(char *s)
{
  _EXITS(s);
}

int _FD2PATH(int fd, const char* s, int len)
{
  Chan *c;
  if(waserror()) return -1;
  c = fdtochan(up->env->fgrp, fd, -1, 0, 1);
  if(c->name != nil){
    snprint(s, len, "%s", c->name->s);
    cclose(c);
  }
  poperror();
  return 0;
}

int fd2path(int fd, const char* s, int len)
{
  return _FD2PATH(fd, s, len);
}

int _FAUTH(int fd, char* name)
{
  return kfauth(fd, name);
}

int _FSESSION(int fd, char* s, int n)
{
  return not_implemented();
}

int _FSTAT(int fd, void* buf, int n)
{
  return kfstat(fd, buf, n);
}

int _FWSTAT(int fd, void* buf, int n)
{
  return kfwstat(fd, buf, n);
}

int _MOUNT(int fd, int afd, const char* old, int flags, const char* spec)
{
  return kmount(fd, afd, (char *)old, flags, (char *)spec);
}

int _NOTED(int i)
{
  return not_implemented();
}

int _NOTIFY(int(*fn)(void*, char*))
{
  return not_implemented();
}

int notify(void(*fn)(void*, char*))
{
  return not_implemented();
}

int _OPEN(const char* path, int mode)
{
  return kopen((char *)path, mode);
}

int _PIPE(int* fds)
{
  return kpipe(fds);
}

long _PREAD(int fd, void* buf, long n, long long off)
{
  return kpread(fd, (void *)buf, n, off);
}
/*
long pread(int fd, void* buf, long n, long long off)
{
  return kpread(fd, (void *)buf, n, off);
}
*/
long _PWRITE(int fd, const void* buf, long n, long long off)
{
  return kpwrite(fd, (void *)buf, n, off);
}
/*
long pwrite(int fd, const void* buf, long n, long long off)
{
  return kpwrite(fd, (void *)buf, n, off);
}
*/

long _READ(int fd, void* buf, long n)
{
  return kread(fd, (void *)buf, n);
}

int _REMOVE(const char* path)
{
  return kremove((char *)path);
}

int _RENDEZVOUS(unsigned long x, unsigned long y)
{
  return not_implemented();
}

int _RFORK(int flags)
{
  return not_implemented();
}

int _SEGATTACH(int n, char* s, void* p, unsigned long l)
{
  return not_implemented();
}

int _SEGBRK(void* p, void* q)
{
  return not_implemented();
}

int _SEGDETACH(void* p)
{
  return not_implemented();
}

int _SEGFLUSH(void* p, unsigned long n)
{
  return not_implemented();
}

int _SEGFREE(void* p, unsigned long n)
{
  return not_implemented();
}

long long _SEEK(int fd, long long off, int whence)
{
  return kseek(fd, off, whence);
}

int _SEMACQUIRE(long* p, int n)
{
  return not_implemented();
}

long _SEMRELEASE(long* p, long n)
{
  return not_implemented();
}

int _SLEEP(long delay)
{
  return not_implemented();
}

int _STAT(const char *path, void *buf, int n)
{
  return kstat(path, buf, n);
}

int _TSEMACQUIRE(long* p, unsigned long n)
{
  return not_implemented();
}

int _UNMOUNT(char *name, char *old)
{
  return not_implemented();
}

long _WRITE(int fd, const void* buf, long n)
{
  return kwrite(fd, buf, n);
}

int _WSTAT(const char* path, void* buf, int n)
{
  return kwstat(path, buf, n);
}

int _CLOCK_GETTIME(int clock_type, void* tv) {
  return clock_gettime(clock_type, (struct timespec *)tv);
}


#if 0

extern int __open(char *path, int mode, ...)
{
  return not_implemented();
}


int __access(char *, int)
{
  return not_implemented();
}


int __chdir(char *)
{
  return kchdir(path)
}


int __creat(char *, int)
{
  return not_implemented();
}


int __link(char *, int)
{
  return not_implemented();
}


int __stat(char *, struct stat *)
{
  return not_implemented();
}


int __unlink(char *) {
}

/*
 * atomic
 */
long ainc(long*);
long adec(long*);
int cas32(unsigned long*, unsigned long, unsigned long);
int casp(void**, void*, void*);
int casl(unsigned long*, unsigned long, unsigned long);

#endif
