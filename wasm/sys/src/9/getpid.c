#include	<lib9.h>
#include	<tos.h>

int
getpid(void)
{
  return _tos->pid;
}
