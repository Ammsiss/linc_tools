#ifndef XFUNCS_H
#define XFUNCS_H

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#define SITE \
    (site_info){ \
        .file = __FILE__, \
        .func = __func__, \
        .line = __LINE__, \
    }

#define XFILE    info->site->file
#define XFUNC    info->site->func
#define XLINE    info->site->line
#define XSYSNAME info->sys_name
#define XERRNO   info->saved_errno

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

#define XFATAL_HANDLER(name) \
    __attribute__ ((__noreturn__)) \
    void name(const xinfo *info [[maybe_unused]])

__attribute__ ((__noreturn__))
typedef void (xfatal_handler)(const xinfo *);

void set_xfatal_handler(xfatal_handler *handler);

#define xopen(...) xopen_at(&SITE, __VA_ARGS__)
int xopen_at(const site_info *site, const char *pathname, int flags, ...);

#define xdup2(...) xdup2_at(&SITE, __VA_ARGS__)
int xdup2_at(const site_info *site, int oldfd, int newfd);

#define xclose(...) xclose_at(&SITE, __VA_ARGS__)
int xclose_at(const site_info *site, int fd);

#define xmalloc(...) xmalloc_at(&SITE, __VA_ARGS__)
void *xmalloc_at(const site_info *site, int size);

#define xrealloc(...) xrealloc_at(&SITE, __VA_ARGS__)
void *xrealloc_at(const site_info *site, void *ptr, int size);

#define xsigaction(...) xsigaction_at(&SITE, __VA_ARGS__)
int xsigaction_at(const site_info *site, int signum,
        const struct sigaction *act, struct sigaction *oldact);

#define xsigemptyset(...) xsigemptyset_at(&SITE, __VA_ARGS__)
int xsigemptyset_at(const site_info *site, sigset_t *set);

#define xsigaddset(...) xsigaddset_at(&SITE, __VA_ARGS__)
int xsigaddset_at(const site_info *site, sigset_t *set, int signum);

#define xsigdelset(...) xsigdelset_at(&SITE, __VA_ARGS__)
int xsigdelset_at(const site_info *site, sigset_t *set, int signum);

#define xsigprocmask(...) xsigprocmask_at(&SITE, __VA_ARGS__)
int xsigprocmask_at(const site_info *site, int how, const sigset_t *set,
        sigset_t *oldset);

#define xsetpgid(...) xsetpgid_at(&SITE, __VA_ARGS__)
int xsetpgid_at(const site_info *site, pid_t pid, pid_t pgid);

#define xtcsetpgrp(...) xtcsetpgrp_at(&SITE, __VA_ARGS__)
int xtcsetpgrp_at(const site_info *site, int fd, pid_t pgrp);

#define xgetcwd(...) xgetcwd_at(&SITE, __VA_ARGS__)
char *xgetcwd_at(const site_info *site, char *buf, size_t size);

#define xkill(...) xkill_at(&SITE, __VA_ARGS__)
int xkill_at(const site_info *site, pid_t pid, int sig);

#define xatexit(...) xatexit_at(&SITE, __VA_ARGS__)
int xatexit_at(const site_info *site, void (*function)(void));

#define xpipe(...) xpipe_at(&SITE, __VA_ARGS__)
int xpipe_at(const site_info *site, int pipefd[2]);

#define xpipe2(...) xpipe2_at(&SITE, __VA_ARGS__)
int xpipe2_at(const site_info *site, int pipefd[2], int flags);

#define xfork() xfork_at(&SITE)
int xfork_at(const site_info *site);

/*
#define x(...) x_at(&SITE, __VA_ARGS__)
int x_at(const site_info *site, );
*/

#endif
