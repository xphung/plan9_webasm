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
CC = ~/Downloads/llvm-build/bin/clang #~/Applications/llvm-wasm/bin/clang
CFLAGS = -fno-signed-char -fno-stack-protector -Wno-shift-negative-value -Wno-comment -Wno-incompatible-pointer-types-discards-qualifiers -Wno-shift-op-parentheses -Wno-parentheses -Wno-bitwise-op-parentheses -Wno-dangling-else -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -target wasm32 #-arch x86_64 #
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
SRC_SUBDIRS += sys/src/libmp sys/src/libsec sys/src/libm

# command line utilities and C compiler
SRC_SUBDIRS += sys/src/9sys sys/src/libbio sys/src/libstring sys/src/cmd #sys/src/libregexp #[regexp needs longjmp/setjmp]

# C99 subset of musl library:
#SRC_SUBDIRS += musl/src/ctype musl/src/env musl/src/errno musl/src/exit musl/src/locale musl/src/multibyte musl/src/prng musl/src/stdlib musl/src/string $(ARCH)/musl/src/signal musl/src/stdio musl/src/temp $(ARCH)/musl/src/time $(ARCH)/musl/src/internal $(ARCH)/musl/src/math $(ARCH)/musl/src/thread $(ARCH)/musl/src/thread # musl/src/fenv musl/src/math musl/src/complex musl/src/setjmp musl/src/malloc_dl

# unix syscalls
#SRC_SUBDIRS += $(ARCH)/musl/src/unistd

SRC_DIRS = $(addprefix $(srcdir)/,$(SRC_SUBDIRS))
ARCH_GLOBS = $(addsuffix /$(ARCH)/*.c,$(SRC_DIRS)) $(addprefix $(srcdir)/$(ARCH)/,$(addsuffix /*.[csS],$(SRC_SUBDIRS)))
LIBC_OBJS = $(ALL_OBJS) #$(filter obj/%,$(ALL_OBJS))
AOBJS = $(LIBC_OBJS)
GENH =

CFLAGS_ALL = $(CFLAGS_C99FSE)
CFLAGS_ALL += -D_POSIX_SOURCE -D_XOPEN_SOURCE=700 #-D_BSD_EXTENSION -D_SUSV2_SOURCE
CFLAGS_ALL += -isystem $(srcdir)/sys/include -isystem $(srcdir)/$(ARCH)/musl/arch -I$(srcdir)/$(ARCH)/musl/src/internal -isystem $(srcdir)/musl/include
CFLAGS_ALL += $(CPPFLAGS) $(CFLAGS_AUTO) $(CFLAGS)

