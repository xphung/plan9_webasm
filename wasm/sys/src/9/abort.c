#include <lib9.h>
void
abort(void)
{
  __builtin_trap();
	//while(*(int*)0)
	//	;
}
