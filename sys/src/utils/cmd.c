#include <lib9.h>
#include <ctype.h>

extern	int	shell_exec(void (*)(void *), void *, char *, char **, int);
extern	int 	main(int, char **);

/*
 * libc routines
 */
/*
#define ctime		kctime
#define asctime		kasctime
#define localtime	klocaltime
#define gmtime		kgmtime
*/
extern  Tm*	gmtime(long);
extern  Tm*	localtime(long);
extern  char*	asctime(Tm*);
extern  char*	ctime(long);
extern  int	needsrcquote(int);

/*
 * commands
 */
 
#define main 		basename_main
#define cmd_source      "cmd/basename.c"
#include                "cmd.h"

#define main		cal_main
#define cmd_source      "cmd/cal.c"
#include                "cmd.h"

//#define main		calls_main
//#define cmd_source      "cmd/calls.c"
//#include                "cmd.h"

#define main		cat_main
#define cmd_source      "cmd/cat.c"
#include                "cmd.h"

#define main		chgrp_main
#define cmd_source      "cmd/chgrp.c"
#include                "cmd.h"

#define main		chmod_main
#define cmd_source      "cmd/chmod.c"
#include                "cmd.h"

#define main		comm_main
#define copy		comm_copy
#define cmd_source      "cmd/comm.c"
#include                "cmd.h"
#undef	copy

#define main		cp_main
#define copy		cp_copy
#define copy1		cp_copy1
#define cmd_source      "cmd/cp.c"
#include                "cmd.h"
#undef	copy
#undef	copy1

#define main		du_main
#define warn		du_warn
#define usage		du_usage
#define cmd_source	"cmd/du.c"
#undef warn
#undef usage
#include                "cmd.h"

#define main		echo_main
#define cmd_source      "cmd/echo.c"
#include                "cmd.h"

//#define main		join_main
//#define cmd_source      "cmd/join.c"
//#include                "cmd.h"

#define main		ls_main
#define cmd_source	"cmd/ls.c"
#include		"cmd.h"

#define main		mv_main
#define samefile	mv_samefile
#define copy1		mv_copy1
#define cmd_source	"cmd/mv.c"
#include		"cmd.h"
#undef 	samefile
#undef 	copy1
/*
#define	main		ns_main
#define usage		ns_usage
#define stdout		ns_stdout
#define cmd_source	"cmd/ns.c"
//#include                "cmd.h"
#undef usage
#undef stdout
*/

#define main 		sh_main
#define fork()		( shell_exec(redirect, np, np->argv[0], &np->argv[0], 2) )
#define exec(c,arg)	(-1)
#define wait(...)	( memcpy(malloc(sizeof(Waitmsg)), &((Waitmsg) {.pid = 2, .msg = "xwait"}), sizeof(Waitmsg)) )
#define waitpid(...)	(-1)
#define Node		sh_Node
#define alloc		sh_alloc
#define error(s,t)	sh_error(s,t)
#define cmd_source      "cmd/sh.C"
#include                "cmd.h"
#undef	fork
#undef	exec
#undef	waitpid
#undef	Node
#undef	alloc
#undef	error

#define	main		sort_main
#define cmd_source	"cmd/sort.c"
#include                "cmd.h"

#define	main		tr_main
#define f		tf_f
#define t		tf_t
#define usage		tr_usage
#define cmd_source	"cmd/tr.c"
#include                "cmd.h"
#undef usage

#define	main		uniq_main
#define skip		uniq_skip
#define cmd_source	"cmd/uniq.c"
#include                "cmd.h"

//#define	main		xd_main
//#define cmd_source	"cmd/xd.c"
//#include                "cmd.h"

int shell_exec(void (*f)(void *), void *f_args, char *cmd, char **argv, int pid) {
  int fd[] =  { _DUP(0, -1), _DUP(1, -1), _DUP(2, -1) };
  int argc = 0;
  while (argv[++argc]);

  void (*m)(int, char**);
  if (!strcmp(argv[0], "basename"))  m = basename_main;
  else if (!strcmp(argv[0], "cal"))  m = cal_main;
  else if (!strcmp(argv[0], "cat"))  m = cat_main;
  //else if (!strcmp(argv[0], "cc"))   m = cc_main;
  else if (!strcmp(argv[0], "comm")) m = comm_main;
  else if (!strcmp(argv[0], "du"))   m = du_main;
  else if (!strcmp(argv[0], "echo")) m = echo_main;
  else if (!strcmp(argv[0], "ls"))   m = ls_main;
  else if (!strcmp(argv[0], "sort")) m = sort_main;
  else return -1;

  f(f_args);
  //if (!setjmp(exit_buf[exit_level++])) {
    m(argc, argv);
  //}

  _DUP(fd[0], 0); _CLOSE(fd[0]);
  _DUP(fd[1], 1); _CLOSE(fd[1]);
  _DUP(fd[2], 2); _CLOSE(fd[2]);
  return pid;
}

#ifdef __wasm32__
#undef main
#include <stdio.h>
extern void _fdinit(char *, char *);

int main(int argc, char *argv[]) {
	_fdinit(0, 0);
	printf("printf working: argc=%d exp(1.0)=%g cos(0.0)=%g\n" , argc, exp(1.0), cos(0.0));
	sh_main(argc, argv);
	return 0;
}
#endif
