#ifndef LINC_TOOLS_XFUNCS_H
#define LINC_TOOLS_XFUNCS_H

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

#define XFILE     info->site->file
#define XFUNC     info->site->func
#define XLINE     info->site->line
#define XSYSNAME  info->sys_name
#define XERRNO    info->saved_errno
#define XPID      info->pid
#define XPPID     info->ppid
#define XPGID     info->pgid
#define XTID      info->tid

#define XFATAL_HANDLER(name) \
    __attribute__ ((__noreturn__)) \
    void name(const _xfuncs_info *info [[maybe_unused]], ...)

/* ONLY USE THE XFUNCS MACROS WITH xfatal_default() IF YOU
 * WILL NOT BE INCLUDED INTO ANOTHER PROJECT THAT MAY
 * ALSO USE THE XFUNCS MACROS WITH xfatal_default().
 *
 * LIBRARIES SHOULD USE x(syscall)_at() FUNCS EXPLICITLY */

#define XFATAL_DEFAULT(xf) \
    xfatal *xfatal_default(void) { \
        assert(xf); \
        return xf; \
    }

#define _XFUNCS_SYS_FAIL(xf, _site, _sys_name, ...) \
    do { \
        assert(xf); \
        if (xf->handler) { \
            _xfuncs_info info = { .site = _site }; \
            _xfuncs_collect_xinfo(&info, #_sys_name); \
            xf->handler(&info __VA_OPT__(,) __VA_ARGS__); \
            abort(); \
        } else \
            exit(EXIT_FAILURE); \
    } while (false)

typedef enum {
    XFUNCS_AVAIL_SID = 1,
    XFUNCS_AVAIL_BTRACE = 2,
} xfuncs_available_flags;

typedef struct {
    const char *file;
    const char *func;
    int line;
} _xfuncs_site;

typedef struct {
    /* Guaranteed */
    int saved_errno;
    const _xfuncs_site *site;
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
} _xfuncs_info;

typedef void (xfatal_handler)(const _xfuncs_info *, ...);

typedef struct {
    xfatal_handler *handler;
} xfatal;

xfatal *xfatal_default(void);

static inline void _xfuncs_collect_xinfo(_xfuncs_info *info,
        const char *sys_name)
{
#define _XFUNCS_BT_BUF_SIZE 100
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
        info->available |= XFUNCS_AVAIL_SID;

    void *bt[_XFUNCS_BT_BUF_SIZE];
    int nptr = backtrace(bt, _XFUNCS_BT_BUF_SIZE);
    if (nptr < _XFUNCS_BT_BUF_SIZE && nptr > 2) {
        info->backtrace = backtrace_symbols(bt + 2, nptr - 2);
        if (info->backtrace) {
            info->backtrace_count = nptr - 2;
            info->available |= XFUNCS_AVAIL_BTRACE;
        }
    }
#undef _XFUNCS_BT_BUF_SIZE
}

#define _XFUNCS_SITE \
    (_xfuncs_site){ \
        .file = __FILE__, \
        .func = __func__, \
        .line = __LINE__, \
    }

#define xopen(...)            _xopen_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xdup2(...)            _xdup2_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xclose(...)           _xclose_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xmalloc(...)          _xmalloc_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xcalloc(...)          _xcalloc_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xrealloc(...)         _xrealloc_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsigaction(...)       _xsigaction_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsigemptyset(...)     _xsigemptyset_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsigaddset(...)       _xsigaddset_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsigdelset(...)       _xsigdelset_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsigprocmask(...)     _xsigprocmask_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsetpgid(...)         _xsetpgid_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xtcsetpgrp(...)       _xtcsetpgrp_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xgetcwd(...)          _xgetcwd_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xkill(...)            _xkill_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xatexit(...)          _xatexit_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xpipe(...)            _xpipe_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xpipe2(...)           _xpipe2_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xfork()               _xfork_at(xfatal_default(), &_XFUNCS_SITE)
#define xtcgetattr(...)       _xtcgetattr_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xtcsetattr(...)       _xtcsetattr_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xforkpty(...)         _xforkpty_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xpoll(...)            _xpoll_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xtimerfd_create(...)  _xtimerfd_create_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xtimerfd_settime(...) _xtimerfd_settime_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xstrdup(...)          _xstrdup_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xstrndup(...)         _xstrndup_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xsetvbuf(...)         _xsetvbuf_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xopendir(...)         _xopendir_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xclosedir(...)        _xclosedir_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xmkdir(...)           _xmkdir_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xrmdir(...)           _xrmdir_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xunlink(...)          _xunlink_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)
#define xchdir(...)           _xchdir_at(xfatal_default(), &_XFUNCS_SITE, __VA_ARGS__)

static inline int
_xopen_at(xfatal *xf, _xfuncs_site *site, const char *pathname, int flags, ...) {
    va_list va;
    mode_t mode = 0;

    if (flags & O_CREAT || (flags & O_TMPFILE) == O_TMPFILE) {
        va_start(va, flags);
        mode = va_arg(va, mode_t);
        va_end(va);
    }

    int rv = open(pathname, flags, mode);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, open, pathname, flags);

    return rv;
}

static inline int
_xdup2_at(xfatal *xf, _xfuncs_site *site, int oldfd, int newfd) {
    int rv = dup2(oldfd, newfd);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, dup2, oldfd, newfd);

    return rv;
}

static inline int
_xclose_at(xfatal *xf, _xfuncs_site *site, int fd) {
    int rv = close(fd);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, close, fd);

    return rv;
}

static inline void *
_xmalloc_at(xfatal *xf, _xfuncs_site *site, int size) {
    void *rv = malloc(size);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, malloc, size);

    return rv;
}

static inline void *
_xcalloc_at(xfatal *xf, _xfuncs_site *site, size_t nmemb, size_t size) {
    void *rv = calloc(nmemb, size);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, calloc, nmemb, size);

    return rv;
}

static inline void *
_xrealloc_at(xfatal *xf, _xfuncs_site *site, void *ptr, int size) {
    void *rv = realloc(ptr, size);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, realloc, ptr, size);

    return rv;
}

static inline int
_xsigaction_at(xfatal *xf, _xfuncs_site *site, int signum,
        const struct sigaction *act, struct sigaction *oldact) {
    int rv = sigaction(signum, act, oldact);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, sigaction, signum, act, oldact);

    return rv;
}

static inline int
_xsigemptyset_at(xfatal *xf, _xfuncs_site *site, sigset_t *set) {
    int rv = sigemptyset(set);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, sigemptyset, set);

    return rv;
}

static inline int
_xsigaddset_at(xfatal *xf, _xfuncs_site *site, sigset_t *set, int signum) {
    int rv = sigaddset(set, signum);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, sigaddset, set, signum);

    return rv;
}

static inline int
_xsigdelset_at(xfatal *xf, _xfuncs_site *site, sigset_t *set, int signum) {
    int rv = sigdelset(set, signum);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, sigdelset, set, signum);

    return rv;
}

static inline int
_xsigprocmask_at(xfatal *xf, _xfuncs_site *site, int how, const sigset_t *set,
        sigset_t *oldset) {
    int rv = sigprocmask(how, set, oldset);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, sigprocmask, how, set, oldset);

    return rv;
}

static inline int
_xsetpgid_at(xfatal *xf, _xfuncs_site *site, pid_t pid, pid_t pgid) {
    int rv = setpgid(pid, pgid);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, setpgid, pid, pgid);

    return rv;
}

static inline int
_xtcsetpgrp_at(xfatal *xf, _xfuncs_site *site, int fd, pid_t pgrp) {
    int rv = tcsetpgrp(fd, pgrp);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, tcsetpgrp, fd, pgrp);

    return rv;
}

static inline char *
_xgetcwd_at(xfatal *xf, _xfuncs_site *site, char *buf, size_t size) {
    char *rv = getcwd(buf, size);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, getcwd, buf, size);

    return rv;
}

static inline int
_xkill_at(xfatal *xf, _xfuncs_site *site, pid_t pid, int sig) {
    int rv = kill(pid, sig);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, kill, pid, sig);

    return rv;
}

static inline int
_xatexit_at(xfatal *xf, _xfuncs_site *site, void (*function)(void)) {
    int rv = atexit(function);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, atexit, function);

    return rv;
}

static inline int
_xpipe_at(xfatal *xf, _xfuncs_site *site, int pipefd[2]) {
    int rv = pipe(pipefd);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, pipe, pipefd);

    return rv;
}

static inline int
_xpipe2_at(xfatal *xf, _xfuncs_site *site, int pipefd[2], int flags) {
    int rv = pipe2(pipefd, flags);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, pipe2, pipefd, flags);

    return rv;
}

static inline int
_xfork_at(xfatal *xf, _xfuncs_site *site) {
    int rv = fork();
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, fork);

    return rv;
}

static inline int
_xtcgetattr_at(xfatal *xf, _xfuncs_site *site, int fd, struct termios *tio) {
    int rv = tcgetattr(fd, tio);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, tcgetattr, fd, tio);

    return rv;
}

static inline int
_xtcsetattr_at(xfatal *xf, _xfuncs_site *site, int fd, int optional_actions,
        const struct termios *termios_p)
{
    int rv = tcsetattr(fd, optional_actions, termios_p);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, tcsetattr, fd, optional_actions, termios_p);

    return rv;
}


static inline int
_xforkpty_at(xfatal *xf, _xfuncs_site *site, int *amaster, char *name,
        const struct termios *tio, const struct winsize *winp) {
    int rv = forkpty(amaster, name, tio, winp);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, forkpty, amaster, name, tio, winp);

    return rv;
}

static inline int
_xpoll_at(xfatal *xf, _xfuncs_site *site, struct pollfd *fds, nfds_t nfds,
        int timeout) {
    int rv = poll(fds, nfds, timeout);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, poll, fds, nfds, timeout);

    return rv;
}

static inline int
_xtimerfd_create_at(xfatal *xf, _xfuncs_site *site, int clockid, int flags) {
    int rv = timerfd_create(clockid, flags);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, timerfd_create, clockid, flags);

    return rv;
}

static inline int
_xtimerfd_settime_at(xfatal *xf, _xfuncs_site *site, int fd, int flags,
        const struct itimerspec *new_value, struct itimerspec *old_value) {
    int rv = timerfd_settime(fd, flags, new_value, old_value);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, timerfd_settime, fd, flags, new_value, old_value);

    return rv;
}

static inline char *
_xstrdup_at(xfatal *xf, _xfuncs_site *site, const char *s) {
    char *rv = strdup(s);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, strdup, s);

    return rv;
}

static inline char *
_xstrndup_at(xfatal *xf, _xfuncs_site *site, const char *s, size_t n) {
    char *rv = strndup(s, n);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, strndup, s, n);

    return rv;
}

static inline int
_xsetvbuf_at(xfatal *xf, _xfuncs_site *site, FILE *stream, char *buf,
        int mode, size_t size) {
    int rv = setvbuf(stream, buf, mode, size);
    if (rv != 0)
        _XFUNCS_SYS_FAIL(xf, site, setvbuf, stream, buf, mode, size);

    return rv;
}

static inline DIR *
_xopendir_at(xfatal *xf, _xfuncs_site *site, const char *name) {
    DIR *rv = opendir(name);
    if (!rv)
        _XFUNCS_SYS_FAIL(xf, site, opendir, name);

    return rv;
}

static inline int
_xclosedir_at(xfatal *xf, _xfuncs_site *site, DIR *dirp) {
    int rv = closedir(dirp);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, closedir, dirp);

    return rv;
}

static inline int
_xmkdir_at(xfatal *xf, _xfuncs_site *site, const char *pathname, mode_t mode) {
    int rv = mkdir(pathname, mode);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, mkdir, pathname, mode);

    return rv;
}

static inline int
_xrmdir_at(xfatal *xf, _xfuncs_site *site, const char *pathname) {
    int rv = rmdir(pathname);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, rmdir, pathname);

    return rv;
}

static inline int
_xunlink_at(xfatal *xf, _xfuncs_site *site, const char *pathname) {
    int rv = unlink(pathname);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, unlink, pathname);

    return rv;
}

static inline int
_xchdir_at(xfatal *xf, _xfuncs_site *site, const char *pathname) {
    int rv = chdir(pathname);
    if (rv == -1)
        _XFUNCS_SYS_FAIL(xf, site, chdir, pathname);

    return rv;
}

#endif
