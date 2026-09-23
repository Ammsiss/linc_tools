#ifndef XFUNCS_H
#define XFUNCS_H

#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <termios.h>
#include <pty.h>
#include <poll.h>
#include <sys/timerfd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <dirent.h>
#include <errno.h>

#define SITE \
    (site_info){ \
        .file = __FILE__, \
        .func = __func__, \
        .line = __LINE__, \
    }

#define XFILE     info->site->file
#define XFUNC     info->site->func
#define XLINE     info->site->line
#define XSYSNAME  info->sys_name
#define XERRNO    info->saved_errno

#define XFATAL_HANDLER(name) \
    __attribute__ ((__noreturn__)) \
    void name(const xinfo *info [[maybe_unused]], ...)

/* ONLY USE THE XFUNCS MACROS WITH xfatal_default() IF YOU
 * WILL NOT BE INCLUDED INTO ANOTHER PROJECT THAT MAY
 * ALSO USE THE XFUNCS MACROS WITH xfatal_default().
 *
 * LIBRARIES SHOULD USE x(syscall)_at() FUNCS EXPLICITLY */

#define XFATAL_DEFAULT(xf) \
    xfatal *xfatal_default(void) { \
        return xf; \
    }

#define SYS_FAIL(xf, _sys_name, ...) \
    do { \
        assert(xf); \
        if (xf->handler) { \
            xinfo info = { .site = site }; \
            collect_xinfo(&info, #_sys_name); \
            xf->handler(&info __VA_OPT__(,) __VA_ARGS__); \
            _exit(EXIT_FAILURE); \
        } else \
            exit(EXIT_FAILURE); \
    } while (false)

typedef enum {
    AVAIL_SID = 1,
    AVAIL_BTRACE = 2,
} available_flags;

typedef struct {
    const char *file;
    const char *func;
    int line;
} site_info;

typedef struct {
    /* Guaranteed */
    int saved_errno;
    const site_info *site;
    const char *sys_name;
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    pid_t tid;

    /* Best-effort */
    uint64_t available;
    pid_t sid;
    int backtrace_count;
    char **backtrace;
} xinfo;

__attribute__ ((__noreturn__))
typedef void (xfatal_handler)(const xinfo *, ...);

typedef struct {
    xfatal_handler *handler;
} xfatal;

xfatal *xfatal_default(void);

static inline void collect_xinfo(xinfo *info, const char *sys_name) {
#define BT_BUF_SIZE 100
    info->saved_errno = errno; /* do this first! */

    /* Guaranteed */

    info->sys_name = sys_name;
    info->pid = getpid();
    info->ppid = getppid();
    info->pgid = getpgrp();
    info->tid = gettid();

    /* Best-effort */

    info->sid = getsid(0);
    if (info->sid != (pid_t)-1)
        info->available |= AVAIL_SID;

    void *bt[BT_BUF_SIZE];
    int nptr = backtrace(bt, BT_BUF_SIZE);
    if (nptr < BT_BUF_SIZE && nptr > 2) {
        info->backtrace = backtrace_symbols(bt + 2, nptr - 2);
        if (info->backtrace) {
            info->backtrace_count = nptr - 2;
            info->available |= AVAIL_BTRACE;
        }
    }
}

#define xopen(...)            xopen_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xdup2(...)            xdup2_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xclose(...)           xclose_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xmalloc(...)          xmalloc_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xcalloc(...)          xcalloc_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xrealloc(...)         xrealloc_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsigaction(...)       xsigaction_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsigemptyset(...)     xsigemptyset_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsigaddset(...)       xsigaddset_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsigdelset(...)       xsigdelset_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsigprocmask(...)     xsigprocmask_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsetpgid(...)         xsetpgid_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xtcsetpgrp(...)       xtcsetpgrp_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xgetcwd(...)          xgetcwd_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xkill(...)            xkill_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xatexit(...)          xatexit_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xpipe(...)            xpipe_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xpipe2(...)           xpipe2_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xfork()               xfork_at(xfatal_default(), &SITE)
#define xtcgetattr(...)       xtcgetattr_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xtcsetattr(...)       xtcsetattr_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xforkpty(...)         xforkpty_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xpoll(...)            xpoll_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xtimerfd_create(...)  xtimerfd_create_at(xfatal_default(),&SITE, __VA_ARGS__)
#define xtimerfd_settime(...) xtimerfd_settime_at(xfatal_default(),&SITE,__VA_ARGS__)
#define xstrdup(...)          xstrdup_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xstrndup(...)         xstrndup_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xsetvbuf(...)         xsetvbuf_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xopendir(...)         xopendir_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xclosedir(...)        xclosedir_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xmkdir(...)           xmkdir_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xrmdir(...)           xrmdir_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xunlink(...)          xunlink_at(xfatal_default(), &SITE, __VA_ARGS__)
#define xchdir(...)           xchdir_at(xfatal_default(), &SITE, __VA_ARGS__)

static inline int
xopen_at(xfatal *xf, const site_info *site, const char *pathname, int flags, ...) {
    va_list va;
    mode_t mode = 0;

    if (flags & O_CREAT || (flags & O_TMPFILE) == O_TMPFILE) {
        va_start(va, flags);
        mode = va_arg(va, mode_t);
        va_end(va);
    }

    int rv = open(pathname, flags, mode);
    if (rv == -1)
        SYS_FAIL(xf, open, pathname, flags);

    return rv;
}

static inline int
xdup2_at(xfatal *xf, const site_info *site, int oldfd, int newfd) {
    int rv = dup2(oldfd, newfd);
    if (rv == -1)
        SYS_FAIL(xf, dup2, oldfd, newfd);

    return rv;
}

static inline int
xclose_at(xfatal *xf, const site_info *site, int fd) {
    int rv = close(fd);
    if (rv == -1)
        SYS_FAIL(xf, close, fd);

    return rv;
}

static inline void *
xmalloc_at(xfatal *xf, const site_info *site, int size) {
    void *rv = malloc(size);
    if (!rv)
        SYS_FAIL(xf, malloc, size);

    return rv;
}

static inline void *
xcalloc_at(xfatal *xf, const site_info *site, size_t nmemb, size_t size) {
    void *rv = calloc(nmemb, size);
    if (!rv)
        SYS_FAIL(xf, calloc, nmemb, size);

    return rv;
}

static inline void *
xrealloc_at(xfatal *xf, const site_info *site, void *ptr, int size) {
    void *rv = realloc(ptr, size);
    if (!rv)
        SYS_FAIL(xf, realloc, ptr, size);

    return rv;
}

static inline int
xsigaction_at(xfatal *xf, const site_info *site, int signum,
        const struct sigaction *act, struct sigaction *oldact) {
    int rv = sigaction(signum, act, oldact);
    if (rv == -1)
        SYS_FAIL(xf, sigaction, signum, act, oldact);

    return rv;
}

static inline int
xsigemptyset_at(xfatal *xf, const site_info *site, sigset_t *set) {
    int rv = sigemptyset(set);
    if (rv == -1)
        SYS_FAIL(xf, sigemptyset, set);

    return rv;
}

static inline int
xsigaddset_at(xfatal *xf, const site_info *site, sigset_t *set, int signum) {
    int rv = sigaddset(set, signum);
    if (rv == -1)
        SYS_FAIL(xf, sigaddset, set, signum);

    return rv;
}

static inline int
xsigdelset_at(xfatal *xf, const site_info *site, sigset_t *set, int signum) {
    int rv = sigdelset(set, signum);
    if (rv == -1)
        SYS_FAIL(xf, sigdelset, set, signum);

    return rv;
}

static inline int
xsigprocmask_at(xfatal *xf, const site_info *site, int how, const sigset_t *set,
        sigset_t *oldset) {
    int rv = sigprocmask(how, set, oldset);
    if (rv == -1)
        SYS_FAIL(xf, sigprocmask, how, set, oldset);

    return rv;
}

static inline int
xsetpgid_at(xfatal *xf, const site_info *site, pid_t pid, pid_t pgid) {
    int rv = setpgid(pid, pgid);
    if (rv == -1)
        SYS_FAIL(xf, setpgid, pid, pgid);

    return rv;
}

static inline int
xtcsetpgrp_at(xfatal *xf, const site_info *site, int fd, pid_t pgrp) {
    int rv = tcsetpgrp(fd, pgrp);
    if (rv == -1)
        SYS_FAIL(xf, tcsetpgrp, fd, pgrp);

    return rv;
}

static inline char *
xgetcwd_at(xfatal *xf, const site_info *site, char *buf, size_t size) {
    char *rv = getcwd(buf, size);
    if (!rv)
        SYS_FAIL(xf, getcwd, buf, size);

    return rv;
}

static inline int
xkill_at(xfatal *xf, const site_info *site, pid_t pid, int sig) {
    int rv = kill(pid, sig);
    if (rv == -1)
        SYS_FAIL(xf, kill, pid, sig);

    return rv;
}

static inline int
xatexit_at(xfatal *xf, const site_info *site, void (*function)(void)) {
    int rv = atexit(function);
    if (rv == -1)
        SYS_FAIL(xf, atexit, function);

    return rv;
}

static inline int
xpipe_at(xfatal *xf, const site_info *site, int pipefd[2]) {
    int rv = pipe(pipefd);
    if (rv == -1)
        SYS_FAIL(xf, pipe, pipefd);

    return rv;
}

static inline int
xpipe2_at(xfatal *xf, const site_info *site, int pipefd[2], int flags) {
    int rv = pipe2(pipefd, flags);
    if (rv == -1)
        SYS_FAIL(xf, pipe2, pipefd, flags);

    return rv;
}

static inline int
xfork_at(xfatal *xf, const site_info *site) {
    int rv = fork();
    if (rv == -1)
        SYS_FAIL(xf, fork);

    return rv;
}

static inline int
xtcgetattr_at(xfatal *xf, const site_info *site, int fd, struct termios *tio) {
    int rv = tcgetattr(fd, tio);
    if (rv == -1)
        SYS_FAIL(xf, tcgetattr, fd, tio);

    return rv;
}

static inline int
xtcsetattr_at(xfatal *xf, const site_info *site, int fd, int optional_actions,
        const struct termios *termios_p)
{
    int rv = tcsetattr(fd, optional_actions, termios_p);
    if (rv == -1)
        SYS_FAIL(xf, tcsetattr, fd, optional_actions, termios_p);

    return rv;
}


static inline int
xforkpty_at(xfatal *xf, const site_info *site, int *amaster, char *name,
        const struct termios *tio, const struct winsize *winp) {
    int rv = forkpty(amaster, name, tio, winp);
    if (rv == -1)
        SYS_FAIL(xf, forkpty, amaster, name, tio, winp);

    return rv;
}

static inline int
xpoll_at(xfatal *xf, const site_info *site, struct pollfd *fds, nfds_t nfds,
        int timeout) {
    int rv = poll(fds, nfds, timeout);
    if (rv == -1)
        SYS_FAIL(xf, poll, fds, nfds, timeout);

    return rv;
}

static inline int
xtimerfd_create_at(xfatal *xf, const site_info *site, int clockid, int flags) {
    int rv = timerfd_create(clockid, flags);
    if (rv == -1)
        SYS_FAIL(xf, timerfd_create, clockid, flags);

    return rv;
}

static inline int
xtimerfd_settime_at(xfatal *xf, const site_info *site, int fd, int flags,
        const struct itimerspec *new_value, struct itimerspec *old_value) {
    int rv = timerfd_settime(fd, flags, new_value, old_value);
    if (rv == -1)
        SYS_FAIL(xf, timerfd_settime, fd, flags, new_value, old_value);

    return rv;
}

static inline char *
xstrdup_at(xfatal *xf, const site_info *site, const char *s) {
    char *rv = strdup(s);
    if (!rv)
        SYS_FAIL(xf, strdup, s);

    return rv;
}

static inline char *
xstrndup_at(xfatal *xf, const site_info *site, const char *s, size_t n) {
    char *rv = strndup(s, n);
    if (!rv)
        SYS_FAIL(xf, strndup, s, n);

    return rv;
}

static inline int
xsetvbuf_at(xfatal *xf, const site_info *site, FILE *stream, char *buf,
        int mode, size_t size) {
    int rv = setvbuf(stream, buf, mode, size);
    if (rv != 0)
        SYS_FAIL(xf, setvbuf, stream, buf, mode, size);

    return rv;
}

static inline DIR *
xopendir_at(xfatal *xf, const site_info *site, const char *name) {
    DIR *rv = opendir(name);
    if (!rv)
        SYS_FAIL(xf, opendir, name, name);

    return rv;
}

static inline int
xclosedir_at(xfatal *xf, const site_info *site, DIR *dirp) {
    int rv = closedir(dirp);
    if (rv == -1)
        SYS_FAIL(xf, closedir, dirp);

    return rv;
}

static inline int
xmkdir_at(xfatal *xf, const site_info *site, const char *pathname, mode_t mode) {
    int rv = mkdir(pathname, mode);
    if (rv == -1)
        SYS_FAIL(xf, mkdir, pathname, mode);

    return rv;
}

static inline int
xrmdir_at(xfatal *xf, const site_info *site, const char *pathname) {
    int rv = rmdir(pathname);
    if (rv == -1)
        SYS_FAIL(xf, rmdir, pathname);

    return rv;
}

static inline int
xunlink_at(xfatal *xf, const site_info *site, const char *pathname) {
    int rv = unlink(pathname);
    if (rv == -1)
        SYS_FAIL(xf, unlink, pathname);

    return rv;
}

static inline int
xchdir_at(xfatal *xf, const site_info *site, const char *pathname) {
    int rv = chdir(pathname);
    if (rv == -1)
        SYS_FAIL(xf, chdir, pathname);

    return rv;
}

#endif
