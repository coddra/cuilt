#ifndef _CUILT_H
#define _CUILT_H

#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>

#ifdef _WIN32
#   include <windows.h>

#   define PATH_SEP "\\"
#elif __linux__
#   define __USE_XOPEN2K
#   include <linux/limits.h>
#   include <unistd.h>

#   define PATH_SEP "/"
#elif __APPLE__
#   include <libproc.h>
#   include <unistd.h>

#   define PATH_SEP "/"
#endif

#define ARG_COUNT(...) ___ARG_COUNT(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, \
    46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, \
    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ___ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
    _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,   \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

typedef struct {
    size_t count;
    char **items;
} strlist;

enum LOG_LEVEL {
    LOG_INFO = 0,
    LOG_WARN = 1,
    LOG_ERROR = 2,
    LOG_FATAL = 3,
};

struct project_config_t {
    char *name;
    char *src_d;
    char *bin_d;
    char *test_d;
    char *build_c;
    char *build_exe;
};
struct cc_config_t {
    char *command;
    strlist flags;
};
struct config_t {
    struct project_config_t project;
    struct cc_config_t cc;
    enum LOG_LEVEL log_level;
};
struct config_t config;
struct config_t default_config();

void msg(enum LOG_LEVEL level, char *fmt, ...);
#define INFO(...) msg(LOG_INFO, __VA_ARGS__)
#define WARN(...) msg(LOG_WARN, __VA_ARGS__)
#define ERROR(...) msg(LOG_ERROR, __VA_ARGS__)
#define FATAL(...) msg(LOG_FATAL, __VA_ARGS__)

#define FOREACH(item, list, body) \
    do { \
        strlist ___list = list; \
        for (size_t ___i = 0; ___i < ___list.count; ___i++) { \
            char *item = ___list.items[___i]; \
            do \
                body \
            while (0); \
        } \
    } while (0)

strlist mklist(size_t count, ...);
strlist listclone(strlist list);
strlist listappend(strlist list, char *item);
strlist listconcat(strlist a, strlist b);
char *listjoin(char *sep, strlist list);
strlist joineach(char *sep, char *body, strlist list);
#define MKLIST(...) mklist(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#ifdef _WIN32
#else
#define PATH_SEP "/"
#endif

#define PATH(...) listjoin(PATH_SEP, MKLIST(__VA_ARGS__))

strlist filesin(char *dir);
bool endswith(char *a, char *b);
strlist filtered(strlist list, char *ext);
char *own_path();
char *cwd();
char *basename(char *path);
char *noext(char *path);
bool modifiedlater(char *p1, char *p2);
#define FILES(dir, ext) filtered(filesin(dir), ext)

int run(strlist cmd);

#endif // _CUILT_H



#ifndef _CUILT_NO_IMPLEMENTATION

// config
struct config_t default_config() {
    struct config_t res = {
        .project = {
            .name = basename(cwd()),
            .src_d = "src",
            .bin_d = "bin",
            .test_d = "test",
            .build_c = "build.c",
            .build_exe = own_path(),
        },
        .cc = {
            .command = "gcc",
            .flags = MKLIST("-Wall", "-Wextra", "-Werror", "-std=c11"),
        },
        .log_level = LOG_INFO,
    };

    return res;
}

// logging
void msg(enum LOG_LEVEL level, char *fmt, ...) {
    if (level < config.log_level)
        return;
    
    switch (level) {
        case LOG_INFO: printf("[INF] "); break;
        case LOG_WARN: printf("[WRN] "); break;
        case LOG_ERROR: printf("[ERR] "); break;
        case LOG_FATAL: printf("[FTL] "); break;
    }

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");

    if (level == LOG_FATAL) {
        fflush(stdout);
        fflush(stderr);
        exit(1);
    }
}

// strlist
strlist mklist(size_t count, ...) {
    strlist res = { 0, NULL };
    res.items = (char **)malloc(sizeof(char *) * count);
    va_list ap;
    va_start(ap, count);
    for (size_t i = 0; i < count; i++)
        res.items[i] = va_arg(ap, char *);
    va_end(ap);
    res.count = count;
    return res;
}

strlist listclone(strlist list) {
    strlist res = { 0, malloc(sizeof(char *) * list.count) };
    memcpy(res.items, list.items, sizeof(char *) * list.count);
    res.count = list.count;
    return res;
}

strlist listappend(strlist list, char *item) {
    list.items = realloc(list.items, sizeof(char *) * (list.count + 1));
    list.items[list.count] = item;
    list.count += 1;
    return list;
}

strlist listconcat(strlist a, strlist b) {
    a.items = realloc(a.items, sizeof(char *) * (a.count + b.count));
    memcpy(a.items + a.count, b.items, sizeof(char *) * b.count);
    a.count += b.count;
    return a;
}

char *listjoin(char *sep, strlist list) {
    size_t sep_len = strlen(sep);
    size_t len = 0;
    char *res = NULL;
    for (size_t i = 0; i < list.count; i++) {
        size_t item_len = strlen(list.items[i]);
        res = realloc(res, len + item_len + sep_len + 1);
        memcpy(res + len, list.items[i], item_len);
        len += item_len;
        if (i < list.count - 1) {
            memcpy(res + len, sep, sep_len);
            len += sep_len;
        }
    }
    res[len] = '\0';
    return res;
}

strlist joineach(char *sep, char *body, strlist list) {
    strlist res = { 0, NULL };
    for (size_t i = 0; i < list.count; i++)
        res = listappend(res, strcat(strcat(body, sep), list.items[i]));
    return res;
}

// path
strlist filesin(char *dir) {
    strlist res = { 0, NULL };
    DIR *d = opendir(dir);
    if (d == NULL)
        return res;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        char *name = de->d_name;
        if (name[0] == '.')
            continue;
        res = listappend(res, PATH(dir, name));
    }
    closedir(d);
    return res;
}

bool endswith(char *a, char *b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a + alen - blen, b, blen) == 0;
}

strlist filtered(strlist list, char *ext) {
    strlist res = { 0, NULL };
    for (size_t i = 0; i < list.count; i++) {
        if (endswith(list.items[i], ext))
            res = listappend(res, list.items[i]);
    }
    return res;
}

char *basename(char *path) {
    char *res = strrchr(path, '/');
    if (res == NULL)
        return path;
    return res + 1;
}

char *noext(char *path) {
    char *tmp = strrchr(path, '.');
    if (tmp == NULL)
        return path;
    char* res = malloc(tmp - path + 1);
    memcpy(res, path, tmp - path);
    return res;
}

char *cwd() {
    char *res = (char*)malloc(PATH_MAX);
    getcwd(res, PATH_MAX);
    return res;
}

bool modifiedlater(char *p1, char *p2)
{
#ifdef _WIN32
    FILETIME p1_time, p2_time;

    Fd p1_fd = fd_open_for_read(p1);
    if (!GetFileTime(p1_fd, NULL, NULL, &p1_time))
        ERROR("Could not get time of %s", p1);
    fd_close(p1_fd);

    Fd p2_fd = fd_open_for_read(p2);
    if (!GetFileTime(p2_fd, NULL, NULL, &p2_time))
        ERROR("Could not get time of %s", p2);
    fd_close(p2_fd);

    return CompareFileTime(&p1_time, &p2_time) == 1;
#else
    struct stat statbuf = {0};

    if (stat(p1, &statbuf) < 0)
        ERROR("Could not get time of %s", p1);
    int p1_time = statbuf.st_mtime;

    if (stat(p2, &statbuf) < 0) 
        ERROR("Could not get time of %s", p2);
    int p2_time = statbuf.st_mtime;

    return p1_time > p2_time;
#endif
}

char* own_path() {
#ifdef _WIN32
    char buffer[MAX_PATH] = {0};
    GetModuleFileName(NULL, buffer, MAX_PATH);
    size_t len = strlen(buffer);
    
#elif __linux__
    char buffer[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
    if (len < 0)
        return NULL;

#elif __APPLE__
    char buffer[PATH_MAX] = {0};
    pid_t pid = getpid();
    if (proc_pidpath(pid, buffer, PATH_MAX) < 0)
        buffer[0] = '\0';
    size_t len = strlen(buffer);
#endif

    char* result = (char*)malloc(len + 1);
    strcpy(result, buffer);
    return result;
}

// run
int run(strlist cmd) {
    if (cmd.count == 0)
        return 0;

    char *strcmd = listjoin(" ", cmd);
    cmd = listappend(listclone(cmd), NULL);

#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    INFO("%s", strcmd);
    if (!CreateProcessA(NULL, strcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        ERROR("Task failed: %s", strcmd);
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (exit_code != 0) {
        ERROR("Task failed: %s", strcmd);
        return exit_code;
    }
#else
    pid_t pid = fork();

    if (pid < 0) {
        ERROR("Task failed: %s", strcmd);
    } else if (pid == 0) {
        INFO("%s", strcmd);
        if (execvp(cmd.items[0], cmd.items) < 0) {
            ERROR("Task failed: %s", strcmd);
            return 1;
        }
    } else {
        int status;
        pid_t wpid = waitpid(pid, &status, 0);

        if (wpid == -1) {
            ERROR("Task failed: %s", strcmd);
            return 1;
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                ERROR("Task failed: %s", strcmd);
                return WEXITSTATUS(status);
            }
        }
        else {
            ERROR("Task failed: %s", strcmd);
            return 1;
        }
    }
#endif
    
    return 0;
}

#define RUN(...) run(MKLIST(__VA_ARGS__))
#define RUNL(files, ...) run(listconcat(MKLIST(__VA_ARGS__), files))

#define CC(files, out) RUNL(listconcat(config.cc.flags, files), config.cc.command, "-o", out)
#define SOURCEFILES FILES(config.project.src_d, ".c")
#define OUTPUT PATH(config.project.bin_d, config.project.name)

// build
#ifndef NO_BUILD
int build(strlist args) {
    CC(SOURCEFILES, OUTPUT);

    return 0;
}
#else
int build(strlist args);
#endif

#ifdef INIT
void init(strlist args);
#endif

int main(int argc, char *argv[]) {
    strlist args = { argc - 1, argv + 1 };
    config = default_config();

#ifdef INIT
    init(args);
#endif

    if (modifiedlater(config.project.build_c, config.project.build_exe)) {
        INFO("Rebuilding...");
        RUN(config.cc.command, "-o", config.project.build_exe, config.project.build_c);
        RUNL(args, config.project.build_exe);
        return 0;
    }

    return build(args);
}

#endif // _CUILT_NO_IMPLEMENTATION