#include "dat.h"
//#include "fns.h"
//#include "error.h"
//#include "interp.h"


#include "emu-g.root.h"

ulong ndevs = 17;

extern Dev rootdevtab;
extern Dev consdevtab;
extern Dev envdevtab;
extern Dev mntdevtab;
extern Dev pipedevtab;
extern Dev procdevtab;
extern Dev snarfdevtab;
extern Dev dupdevtab;
extern Dev ssldevtab;
extern Dev indirdevtab;
Dev* devtab[]={
	&rootdevtab,
	&consdevtab,
	&envdevtab,
	&mntdevtab,
	&pipedevtab,
	&procdevtab,
	&snarfdevtab,
	&dupdevtab,
  &indirdevtab,
	nil, //&ssldevtab,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
	nil,
};

void links(void){
}

void modinit(void){
}

	int	dontcompile = 1;
	int macjit = 1;
	void setpointer(int x, int y){USED(x); USED(y);}
	ulong strtochan(char *s){USED(s); return ~0;}
char* conffile = "emu-g";
ulong kerndate = 1543009423;
