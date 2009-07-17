/* stdenv.hxx -- set up an environment we can use ($Revision: 1.3 $) */

#include "esconfig.hxx"
#ifdef HAVE_SYS_CDEFS_H
# include <sys/cdefs.h>
#endif

/*
 * type qualifiers
 */

#if !USE_VOLATILE
# ifndef volatile
#  define volatile
# endif
#endif


/*
 * protect the rest of es source from the dance of the includes
 */

#if HAVE_UNISTD_H
#include <unistd.h>
#endif


#include <string.h>
#include <stddef.h>

#if HAVE_MEMORY_H
#include <memory.h>
#endif

#if HAVE_STDARG_H
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <ctype.h>

#if REQUIRE_STAT || REQUIRE_IOCTL
#include <sys/types.h>
#endif

#if REQUIRE_IOCTL
#include <sys/ioctl.h>
#endif

#if REQUIRE_STAT
#include <sys/stat.h>
#endif

#if __GNUC__
#define NORETURN __attribute__ ((noreturn))
#else
#define NORETURN
#endif

#if REQUIRE_DIRENT
#if HAVE_DIRENT_H
#include <dirent.h>
typedef struct dirent Dirent;
#else
#include <sys/dir.h>
typedef struct direct Dirent;
#endif
#endif

#include <sys/wait.h>

/* stdlib */

#if STDC_HEADERS
# include <stdlib.h>
#else
extern exit(int) NORETURN;
extern abort(void) NORETURN;
extern long strtol(const char *num, char **end, int base);
extern void *qsort(
	void *base, size_t nmemb, size_t size,
	int (*compar)(const void *, const void *)
);
#endif /* !STDC_HEADERS */

/*
 * things that should be defined by header files but might not have been
 */

#ifndef	offsetof
#define	offsetof(t, m)	((size_t) (((char *) &((t *) 0)->m) - (char *)0))
#endif

#ifndef	EOF
#define	EOF	(-1)
#endif


/*
 * macros
 */

#define	streq(s, t)		(strcmp(s, t) == 0)
#define	strneq(s, t, n)		(strncmp(s, t, n) == 0)
#define	hasprefix(s, p)		strneq(s, p, (sizeof p) - 1)
#define	arraysize(a)		((int) (sizeof (a) / sizeof (*a)))
#define	memzero(dest, count)	memset(dest, 0, count)

#define	STMT(stmt)		do { stmt; } while (0)
#define	NOP			do {} while (0)

#define CONCAT(a,b)	a ## b

/* No macro expansion */
#define STRING(s)	#s

/*
 * types we use throughout es
 */

#if USE_SIG_ATOMIC_T
typedef volatile sig_atomic_t Atomic;
#else
typedef volatile int Atomic;
#endif

#ifdef VOID_SIGNALS
typedef void Sigresult;
#define	SIGRESULT
#else
typedef int Sigresult;
#define	SIGRESULT	0
#endif

typedef GETGROUPS_T gidset_t;

/*
 * assertion checking
 */

#if ASSERTIONS
#define	assert(expr) \
	STMT( \
		if (!(expr)) { \
			eprint("%s:%d: assertion failed (%s)\n", \
				__FILE__, __LINE__, STRING(expr)); \
			abort(); \
		} \
	)
#else
#define	assert(ignore)	NOP
#endif

enum { UNREACHABLE = 0 };


#define	NOTREACHED	STMT(assert(UNREACHABLE))

/*
 * system calls -- can we get these from some standard header uniformly?
 */

#if !HAVE_UNISTD_H
extern int chdir(const char *dirname);
extern int close(int fd);
extern int dup(int fd);
extern int dup2(int srcfd, int dstfd);
extern int execve(char *name, char **argv, char **envp);
extern int fork(void);
extern int getegid(void);
extern int geteuid(void);
extern int getpagesize(void);
extern int getpid(void);
extern int pipe(int p[2]);
extern int read(int fd, void *buf, size_t n);
extern int setpgrp(int pid, int pgrp);
extern int umask(int mask);
extern int write(int fd, const void *buf, size_t n);

#if REQUIRE_IOCTL
extern int ioctl(int fd, int cmd, void *arg);
#endif

#if REQUIRE_STAT
extern int stat(const char *, struct stat *);
#endif

#ifdef NGROUPS
extern int getgroups(int, int *);
#endif
#endif	/* !HAVE_UNISTD_H */


/*
 * hacks to present a standard system call interface
 */

#ifdef HAVE_SETSID
# define setpgrp(a, b)	setsid()
#else
#ifdef linux
#include "unistd.h"
#define setpgrp(a, b)	setpgid(a, b)
#endif

#if sgi
#define	setpgrp(a, b)	BSDsetpgrp(a,b)
#endif

#if HPUX
#define	setpgrp(a, b)	setpgrp()
#endif
#endif

#if !HAVE_LSTAT
#define	lstat	stat
#endif

#ifndef LISPTREES
#define LISPTREES 1
#endif
