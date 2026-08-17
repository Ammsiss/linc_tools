#define _GNU_SOURCE

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <execinfo.h>

#include "xfuncs.h"

#define BT_BUF_SIZE 100

#define SYS_FAIL(_sys_name) \
    do { \
        if (xfatal) { \
            xinfo info = { .site = site }; \
            collect_xinfo(&info, #_sys_name); \
            xfatal(&info); \
        } else \
            exit(EXIT_FAILURE); \
    } while (false)

static void collect_xinfo(xinfo *info, const char *sys_name) {
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

static xfatal_handler *xfatal = NULL;

void set_xfatal_handler(xfatal_handler *handler) {
    assert(handler);
    xfatal = handler;
}

int xopen_at(const site_info *site, const char *pathname, int flags, ...) {
    va_list va;
    mode_t mode = 0;

    if (flags & O_CREAT || (flags & O_TMPFILE) == O_TMPFILE) {
        va_start(va, flags);
        mode = va_arg(va, mode_t);
        va_end(va);
    }

    int rv = open(pathname, flags, mode);
    if (rv == -1)
        SYS_FAIL(open);

    return rv;
}

int xdup2_at(const site_info *site, int oldfd, int newfd) {
    int rv = dup2(oldfd, newfd);
    if (rv == -1)
        SYS_FAIL(dup2);

    return rv;
}

int xclose_at(const site_info *site, int fd) {
    int rv = close(fd);
    if (rv == -1)
        SYS_FAIL(close);

    return rv;
}

void *xmalloc_at(const site_info *site, int size) {
    void *rv = malloc(size);
    if (!rv)
        SYS_FAIL(malloc);

    return rv;
}

void *xrealloc_at(const site_info *site, void *ptr, int size) {
    void *rv = realloc(ptr, size);
    if (!rv)
        SYS_FAIL(realloc);

    return rv;
}

int xsigaction_at(const site_info *site, int signum,
        const struct sigaction *act, struct sigaction *oldact) {
    int rv = sigaction(signum, act, oldact);
    if (rv == -1)
        SYS_FAIL(sigaction);

    return rv;
}

int xsigemptyset_at(const site_info *site, sigset_t *set) {
    int rv = sigemptyset(set);
    if (rv == -1)
        SYS_FAIL(sigemptyset);

    return rv;
}

int xsigaddset_at(const site_info *site, sigset_t *set, int signum) {
    int rv = sigaddset(set, signum);
    if (rv == -1)
        SYS_FAIL(sigaddset);

    return rv;
}

int xsigdelset_at(const site_info *site, sigset_t *set, int signum) {
    int rv = sigdelset(set, signum);
    if (rv == -1)
        SYS_FAIL(sigdelset);

    return rv;
}

int xsigprocmask_at(const site_info *site, int how, const sigset_t *set,
        sigset_t *oldset) {
    int rv = sigprocmask(how, set, oldset);
    if (rv == -1)
        SYS_FAIL(sigprocmask);

    return rv;
}

int xsetpgid_at(const site_info *site, pid_t pid, pid_t pgid) {
    int rv = setpgid(pid, pgid);
    if (rv == -1)
        SYS_FAIL(setpgid);

    return rv;
}

int xtcsetpgrp_at(const site_info *site, int fd, pid_t pgrp) {
    int rv = tcsetpgrp(fd, pgrp);
    if (rv == -1)
        SYS_FAIL(tcsetpgrp);

    return rv;
}

char *xgetcwd_at(const site_info *site, char *buf, size_t size) {
    char *rv = getcwd(buf, size);
    if (!rv)
        SYS_FAIL(getcwd);

    return rv;
}

int xkill_at(const site_info *site, pid_t pid, int sig) {
    int rv = kill(pid, sig);
    if (rv == -1)
        SYS_FAIL(kill);

    return rv;
}

int xatexit_at(const site_info *site, void (*function)(void)) {
    int rv = atexit(function);
    if (rv == -1)
        SYS_FAIL(atexit);

    return rv;
}

int xpipe_at(const site_info *site, int pipefd[2]) {
    int rv = pipe(pipefd);
    if (rv == -1)
        SYS_FAIL(pipe);

    return rv;
}

int xpipe2_at(const site_info *site, int pipefd[2], int flags) {
    int rv = pipe2(pipefd, flags);
    if (rv == -1)
        SYS_FAIL(pipe2);

    return rv;
}

int xfork_at(const site_info *site) {
    int rv = fork();
    if (rv == -1)
        SYS_FAIL(fork);

    return rv;
}

/*
    int rv = ();
    if (rv == -1) {
        if (xfatal) {
            xinfo info = { .site = site };
            collect_xinfo(&info, "");
            xfatal(&info);
        } else
            exit(EXIT_FAILURE);
    }

    return rv;
*/
