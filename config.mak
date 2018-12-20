#
#  This requires wasm32 clang installed at ~/Downloads/llvm-build/bin/clang
#
#  To build, type:
#   make
#
#  To deploy, use webserver.py script to start mini-webserver with wasm mimetype configured
#  To run the server, type:
#   ./webserver.py
#
#  Then open http://localhost:8001/web/shell.html in Chrome browser
#

ARCH = wasm
SUBARCH = 
ASMSUBARCH = 
srcdir = .
prefix = ./proFile
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(prefix)/lib
includedir = $(prefix)/include
syslibdir = /lib
CC = ~/Downloads/llvm-build/bin/clang
CFLAGS = -fno-signed-char -fno-stack-protector -Winline -Wno-shift-negative-value -Wno-comment -Wno-string-plus-int -Wno-incompatible-pointer-types-discards-qualifiers -Wno-shift-op-parentheses -Wno-parentheses -Wno-bitwise-op-parentheses -Wno-dangling-else -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -target wasm32 #-arch x86_64 #
CFLAGS_AUTO = -O2 -fno-unwind-tables -fno-asynchronous-unwind-tables -Werror=implicit-function-declaration -Werror=pointer-sign -Werror=pointer-arith -DKERNDATE=1543378996 -fomit-frame-pointer #-g
CFLAGS_MEMOPS = 
CPPFLAGS = 
LDFLAGS = -Xlinker --export=kmain -Xlinker --export=main -Xlinker --allow-undefined #-Xlinker -error-limit=0 # -Xlinker --no-entry 
LDFLAGS_AUTO = 
CROSS_COMPILE = ~/Downloads/llvm-build/bin/llvm-
LIBCC = 
OPTIMIZE_GLOBS = internal/*.c malloc/*.c string/*.c
ALL_TOOLS = 
TOOL_LIBS = 
ADD_CFI = no
AOBJS = $(LOBJS)
SHARED_LIBS =

# kernel and libbio:
SRC_SUBDIRS = sys/src/9 sys/src/fmt sys/src/libc

# devssl needs libsec (which needs libmp):
SRC_SUBDIRS += sys/src/libmp sys/src/libsec

# command line utilities and C compiler ... can omit these for ANSI/POSIX code not needing Plan9 libraries
SRC_SUBDIRS += sys/src/9sys sys/src/libbio sys/src/libstring sys/src/utils #sys/src/libregexp #[regexp needs longjmp/setjmp]

# Plan9 math library, can use this instead of musl math library
SRC_SUBDIRS += sys/src/libm

###
### musl C11 subset:
###
###   complex ctype errno env/*env.c fenv locale math multibyte prng setjmp stdlib string:
###			no direct syscalls
###   stdio (streams):
###			sys_open SYS_close SYS_fcntl SYS_ioctl SYS_lseek SYS_read SYS_readv SYS_writev
###   stdio (fileops):
###			SYS_lstat SYS_rename SYS_rmdir SYS_unlink SYS_wait4
###   exit:
###			SYS_exit SYS_exit_group SYS_rt_sigaction SYS_rt_sigprocmask SYS_tkill
###   signal/raise:
###			SYS_tkill
###   signal/sigaction:
###			SYS_rt_sigaction SYS_rt_sigprocmask
###   time:
###			__map_file SYS_fstat SYS_close

SRC_SUBDIRS += musl/src/ctype musl/src/env musl/src/errno musl/src/exit musl/src/locale musl/src/multibyte musl/src/prng musl/src/stdlib musl/src/string $(ARCH)/musl/src/signal musl/src/stdio musl/src/temp $(ARCH)/musl/src/time $(ARCH)/musl/src/internal $(ARCH)/musl/src/math $(ARCH)/musl/src/thread #musl/src/fenv musl/src/math musl/src/complex #musl/src/setjmp

###
### musl POSIX library:
###
###   crypt passwd regex search temp
###			[no direct syscalls]
###   aio:
###			SYS_rt_sigqueueinfo
###   conf:
###			SYS_ SYS_sched_getaffinity 
###   dirent:
###			SYS_close SYS_getdents 
###   env/init_tls.c:
###			SYS_mmap SYS_mmap2 SYS_poll SYS_ppoll SYS_set_tid_address
###   fcntl:
###			SYS_close SYS_fadvise SYS_fallocate SYS_fcntl SYS_openat 
###   select:
###			SYS_poll SYS_ppoll SYS_pselect6 SYS_select 
###   stat:
###			SYS_chmod SYS_close SYS_fchmod SYS_fchmodat SYS_fcntl
###			SYS_fstat SYS_fstatat SYS_fstatfs SYS_fstatfs64 SYS_futimesat SYS_lstat
###			SYS_mkdir SYS_mkdirat SYS_mknod SYS_mknodat SYS_openat
###			SYS_stat SYS_statfs SYS_statfs64 SYS_umask SYS_utimensat SYS_utimes 
###   signal:
###			SYS_getitimer SYS_kill SYS_rt_sigaction SYS_rt_sigpending SYS_rt_sigprocmask
###			SYS_rt_sigqueueinfo SYS_rt_sigreturn SYS_rt_sigsuspend SYS_rt_sigtimedwait SYS_setitimer SYS_sigaltstack 
###   termios:
###			SYS_ioctl 
###   time (clock):
###			SYS_clock_getres SYS_clock_gettime SYS_clock_settime SYS_clock_nanosleep SYS_times
###   time (timer):
###			SYS_rt_sigprocmask SYS_timer_create SYS_timer_delete SYS_timer_getoverrun SYS_timer_gettime SYS_timer_settime
###   unistd:
###			SYS_access SYS_acct SYS_chdir SYS_chown SYS_close SYS_dup SYS_dup2 SYS_dup3 SYS_exit
###			SYS_faccessat SYS_fchdir SYS_fchown SYS_fchownat SYS_fcntl SYS_fdatasync SYS_fsync SYS_ftruncate
###			SYS_getcwd SYS_getegid SYS_geteuid SYS_getgid SYS_getgroups SYS_getpgid SYS_getpid SYS_getppid SYS_getsid SYS_getuid
###			SYS_ioctl SYS_kill SYS_lchown SYS_link SYS_linkat SYS_lseek SYS_pause SYS_pipe SYS_pipe2 SYS_ppoll
###			SYS_pread SYS_preadv SYS_pwrite SYS_pwritev SYS_read SYS_readlink SYS_readlinkat SYS_readv SYS_renameat SYS_rmdir
###			SYS_setgid SYS_setitimer SYS_setpgid SYS_setregid SYS_setresgid SYS_setresuid SYS_setreuid SYS_setsid SYS_setuid
###			SYS_symlink SYS_symlinkat SYS_sync SYS_truncate SYS_unlink SYS_unlinkat SYS_wait4 SYS_write SYS_writev
###   ipc mman mq:
###			SYS_close SYS_ipc
###			SYS_madvise SYS_mincore SYS_mlock SYS_mlockall SYS_mmap SYS_mmap2 SYS_mprotect SYS_mremap 
###			SYS_msync SYS_munlock SYS_munlockall SYS_munmap
###			SYS_mq_getsetattr SYS_mq_notify SYS_mq_open SYS_mq_timedreceive SYS_mq_timedsend SYS_mq_unlink
###			SYS_msgctl SYS_msgget SYS_msgrcv SYS_msgsnd
###			SYS_semctl SYS_semget SYS_semop SYS_semtimedop
###			SYS_shmat SYS_shmctl SYS_shmdt SYS_shmget 

SRC_SUBDIRS += musl/src/crypt musl/src/search musl/src/temp $(ARCH)/musl/src/unistd #musl/src/regex #musl/src/passwd

SRC_DIRS = $(addprefix $(srcdir)/,$(SRC_SUBDIRS))
ARCH_GLOBS = $(addsuffix /$(ARCH)/*.c,$(SRC_DIRS)) $(addprefix $(srcdir)/$(ARCH)/,$(addsuffix /*.[csS],$(SRC_SUBDIRS)))
LIBC_OBJS = $(ALL_OBJS) #$(filter obj/%,$(ALL_OBJS))
AOBJS = $(LIBC_OBJS)
GENH =

CFLAGS_ALL = $(CFLAGS_C99FSE)
CFLAGS_ALL += -D_POSIX_SOURCE -D_XOPEN_SOURCE=700 #-D_BSD_EXTENSION -D_SUSV2_SOURCE
CFLAGS_ALL += -isystem $(srcdir)/sys/include -isystem $(srcdir)/$(ARCH)/musl/arch -I$(srcdir)/$(ARCH)/musl/src/internal -isystem $(srcdir)/musl/include
CFLAGS_ALL += $(CPPFLAGS) $(CFLAGS_AUTO) $(CFLAGS)

