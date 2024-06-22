// cuilt.h
/*
Indie Developer License (IDL) 1.0

Copyright (c) 2024 Nádas András <andrew.reeds.mpg@gmail.com>

Permission is hereby granted, free of charge, to any individual developer or 
any group of up to 12 developers (collectively, "Developers") obtaining a copy 
of this software and associated documentation files (the "Software"), to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
Software, subject to the following conditions:

Developers must provide proper credit, including the original author's name, a 
link to the original source, and an indication of any changes made to the 
Software. This credit must be included in the software's documentation, in the 
"About" section, or any other prominent location in the distribution.

Credit must not suggest the author endorses the Developer or their use of the 
Software.

Any company, organization, or group of more than 12 individuals (collectively, 
"Entities") are strictly prohibited from using the Software for commercial 
purposes without obtaining a separate commercial license from the copyright 
holder.

Entities desiring to use the Software for commercial purposes must contact the 
copyright holder to negotiate terms and obtain a commercial license.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

By using the Software, Developers and Entities agree to the terms of this 
license.
*/

#ifndef _CUILT_H
#define _CUILT_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#   include <windows.h>
#   include <io.h>

#   define access _access
#   define F_OK 0
#   define PATH_SEP "\\"
#   define PATH_MAX MAX_PATH
#elif __linux__
#   ifndef __USE_XOPEN2K
#       define __USE_XOPEN2K
#   endif
#   include <dirent.h>
#   include <linux/limits.h>
#   include <unistd.h>
#   include <sys/wait.h>

#   define PATH_SEP "/"
#elif __APPLE__
#   include <dirent.h>
#   include <libproc.h>
#   include <unistd.h>
#   include <sys/wait.h>

#   define PATH_SEP "/"
#endif

#define HEAD(a, ...) a
#define TAIL(a, ...) __VA_ARGS__
#define FIRST(...) HEAD(__VA_ARGS__)
#define SECOND(...) FIRST(TAIL(__VA_ARGS__))
#define ___CAT(a, b) a##b
#define CAT(a, b) ___CAT(a, b)
#define ___NOT0() 1, 1
#define ___NOT() 1, 1
#define NOT(a) SECOND(CAT(___NOT, a)(), 0)
#define BOOL(a) NOT(NOT(a))
#define ___IF_ELSE0(...) ___ELSE0
#define ___IF_ELSE1(...) __VA_ARGS__ ___ELSE1
#define ___ELSE0(...) __VA_ARGS__
#define ___ELSE1(...)
#define IF_ELSE(condition) CAT(___IF_ELSE, BOOL(condition))
#define ___HAS_ARGS() 0
#define HAS_ARGS(...) BOOL(HEAD(___HAS_ARGS __VA_ARGS__)())
#define ARG_COUNT(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(___ARG_COUNT(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, \
    46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, \
    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))(0)
#define ___ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
    _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,   \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

typedef struct {
    size_t count;
    const char** items;
} strlist;

enum LOG_LEVEL {
    LOG_INFO = 0,
    LOG_WARN = 1,
    LOG_ERROR = 2,
    LOG_FATAL = 3,
};

typedef int (*process_t)(strlist argv);

struct process_config_t {
    process_t init;
    process_t build;
    process_t run;
    process_t test;
    process_t clean;
    strlist passthrough;
};
struct project_config_t {
    const char* name;
    const char* src;
    const char* bin;
    const char* test;
    const char* do_c;
    const char* do_exe;
};
struct cc_config_t {
    const char* command;
    strlist flags;
};
struct config_t {
    struct project_config_t project;
    struct cc_config_t cc;
    struct process_config_t process;
    enum LOG_LEVEL log_level;
};
extern struct config_t config;
struct config_t default_config(void);
struct config_t ___config(void);
struct config_t merge_config(struct config_t a, struct config_t b);
#define CONFIG(...) struct config_t ___config(void) { \
    struct config_t res = __VA_ARGS__; \
    res.project.do_c = __FILE__; \
    return res; \
}

void msg(enum LOG_LEVEL level, const char* fmt, ...);
#define INFO(...) msg(LOG_INFO, __VA_ARGS__)
#define WARN(...) msg(LOG_WARN, __VA_ARGS__)
#ifdef ERROR
#undef ERROR
#endif
#define ERROR(...) msg(LOG_ERROR, __VA_ARGS__)
#define FATAL(...) msg(LOG_FATAL, __VA_ARGS__)

#define FOREACH(item, list, body) \
    do { \
        strlist ___list = list; \
        for (size_t ___i = 0; ___i < ___list.count; ___i++) { \
            const char* item = ___list.items[___i]; \
            do \
                body \
            while (0); \
        } \
    } while (0)

strlist mklist(size_t count, ...);
strlist clone(strlist list);
strlist append(strlist list, const char* item);
strlist concat(strlist a, strlist b);
strlist remove_item(strlist list, size_t item);
char* join(const char* sep, strlist list);
strlist joineach(const char* sep, const char* body, strlist list);
#define LIST(...) mklist(ARG_COUNT(__VA_ARGS__) IF_ELSE(HAS_ARGS(__VA_ARGS__))(, __VA_ARGS__)())

#define PATH(...) join(PATH_SEP, LIST(__VA_ARGS__))

strlist filesin(const char* dir);
bool endswith(const char* a, const char* b);
strlist filtered(strlist list, const char* ext);
bool exists(const char* path);
char* own_path(void);
char* cwd(void);
const char* basename(const char* path);
const char* noext(const char* path);
bool modifiedlater(const char* p1, const char* p2);
#define FILES(dir, ext) filtered(filesin(dir), ext)

int run(strlist cmd);

#define RUN(...) run(LIST(__VA_ARGS__))
#define RUNL(files, ...) run(concat(LIST(__VA_ARGS__), files))

#define CC(files, out) RUNL(concat(config.cc.flags, files), config.cc.command, "-o", out)

#ifndef SOURCEFILES
#define SOURCEFILES FILES(config.project.src, ".c")
#endif

#ifndef OUTPUT
#define OUTPUT PATH(config.project.bin, config.project.name)
#endif


#endif // _CUILT_H



#ifndef _CUILT_NO_IMPLEMENTATION

// config
int ___build(strlist argv);
int ___run(strlist argv);
struct config_t config;
struct config_t default_config(void) {
    struct config_t res = {
        .project = {
            .name = basename(cwd()),
            .src = "src",
            .bin = "bin",
            .test = "test",
            .do_c = NULL,
            .do_exe = own_path(),
        },
        .cc = {
            .command = "cc",
            .flags = LIST("-Wall", "-Wextra", "-Werror", "-std=c11"),
        },
        .process = {
            .init = NULL,
            .build = &___build,
            .run = &___run,
            .test = NULL,
            .clean = NULL,
            .passthrough = { 0, NULL },
        },
        .log_level = LOG_INFO,
    };
    return res;
}
struct config_t merge_config(struct config_t a, struct config_t b) {
    struct config_t res = a;
    if (b.project.name) res.project.name = b.project.name;
    if (b.project.src) res.project.src = b.project.src;
    if (b.project.bin) res.project.bin = b.project.bin;
    if (b.project.test) res.project.test = b.project.test;
    if (b.project.do_c) res.project.do_c = b.project.do_c;
    if (b.project.do_exe) res.project.do_exe = b.project.do_exe;
    if (b.cc.command) res.cc.command = b.cc.command;
    if (b.cc.flags.items) res.cc.flags = b.cc.flags;
    if (b.process.init) res.process.init = b.process.init;
    if (b.process.build) res.process.build = b.process.build;
    if (b.process.run) res.process.run = b.process.run;
    if (b.process.test) res.process.test = b.process.test;
    if (b.process.clean) res.process.clean = b.process.clean;
    if (b.process.passthrough.items) res.process.passthrough = b.process.passthrough;
    if (b.log_level) res.log_level = b.log_level;
    return res;
}

// logging
void msg(enum LOG_LEVEL level, const char* fmt, ...) {
    if (level < config.log_level)
        return;
    
    switch (level) {
        case LOG_INFO: fputs("[INF] ", stderr); break;
        case LOG_WARN: fputs("[WRN] ", stderr); break;
        case LOG_ERROR: fputs("[ERR] ", stderr); break;
        case LOG_FATAL: fputs("[FTL] ", stderr); break;
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);

    if (level == LOG_FATAL) {
        fflush(stdout);
        fflush(stderr);
        exit(1);
    }
}

// strlist
strlist mklist(size_t count, ...) {
    strlist res = { 0, NULL };
    res.items = (const char**)malloc(sizeof(char*) * count);
    va_list ap;
    va_start(ap, count);
    for (size_t i = 0; i < count; i++)
        res.items[i] = va_arg(ap, char*);
    va_end(ap);
    res.count = count;
    return res;
}

strlist clone(strlist list) {
    strlist res = { 0, (const char**)malloc(sizeof(char*) * list.count) };
    memcpy(res.items, list.items, sizeof(char*) * list.count);
    res.count = list.count;
    return res;
}

strlist append(strlist list, const char* item) {
    list.items = (const char**)realloc(list.items, sizeof(char*) * (list.count + 1));
    list.items[list.count] = item;
    list.count += 1;
    return list;
}

strlist concat(strlist a, strlist b) {
    a.items = (const char**)realloc(a.items, sizeof(char*) * (a.count + b.count));
    memcpy(a.items + a.count, b.items, sizeof(char*) * b.count);
    a.count += b.count;
    return a;
}

strlist remove_item(strlist list, size_t item) {
    list.items = (const char**)realloc(list.items, sizeof(char*) * (list.count - 1));
    memmove(list.items + item, list.items + item + 1, sizeof(char*) * (list.count - item - 1));
    list.count -= 1;
    return list;
}

char* join(const char* sep, strlist list) {
    size_t sep_len = strlen(sep);
    size_t len = 0;
    char* res = malloc(1 * sizeof(char));
    for (size_t i = 0; i < list.count; i++) {
        size_t item_len = strlen(list.items[i]);
        res = (char*)realloc(res, len + item_len + sep_len + 1);
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

strlist joineach(const char* sep, const char* body, strlist list) {
    strlist res = { 0, NULL };
    for (size_t i = 0; i < list.count; i++) {
        char* item = (char*)malloc(1 * sizeof(char));
        *item = '\0';
        res = append(res, strcat(strcat(strcat(item, body), sep), list.items[i]));
    }
    return res;
}

strlist split(const char* sep, const char* body) {
    strlist res = { 0, NULL };
    char* tmp = (char*)malloc(strlen(body) + 1);
    memcpy(tmp, body, strlen(body) + 1);
    char* p = strtok(tmp, sep);
    while (p) {
        res = append(res, p);
        p = strtok(NULL, sep);
    }
    return res;
}

strlist filesin(const char* dir) {
    strlist res = { 0, NULL };
#ifdef _WIN32
    WIN32_FIND_DATA data;
    HANDLE h = FindFirstFile(PATH(dir, "*"), &data);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName, ".") != 0 && strcmp(data.cFileName, "..") != 0)
                res = append(res, PATH(dir, data.cFileName));
        } while (FindNextFile(h, &data));
        FindClose(h);
    }
#else
    DIR* d = opendir(dir);
    if (d != NULL) {
        struct dirent* de;
        while ((de = readdir(d)) != NULL) {
            char* name = de->d_name;
            if (name[0] != '.')
                res = append(res, PATH(dir, name));
        }
        closedir(d);
    }
#endif
    return res;
}

bool endswith(const char* a, const char* b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a + alen - blen, b, blen) == 0;
}

strlist filtered(strlist list, const char* ext) {
    strlist res = { 0, NULL };
    for (size_t i = 0; i < list.count; i++) {
        if (endswith(list.items[i], ext))
            res = append(res, list.items[i]);
    }
    return res;
}

const char* basename(const char* path) {
    const char* res = strrchr(path, '/');
    if (res == NULL)
        return path;
    return res + 1;
}

const char* noext(const char* path) {
    const char* tmp = strrchr(path, '.');
    if (tmp == NULL)
        return path;
    char* res = (char*)malloc(tmp - path + 1);
    memcpy(res, path, tmp - path);
    return res;
}

char* cwd(void) {
    char* res = (char*)malloc(PATH_MAX);
#ifdef _WIN32
    GetCurrentDirectoryA(PATH_MAX, res);
#elif __linux__
    getcwd(res, PATH_MAX);
#elif __APPLE__
    char tmp[PATH_MAX];
    getcwd(tmp, PATH_MAX);
    realpath(tmp, res);
#endif
    return res;
}

bool modifiedlater(const char* p1, const char* p2)
{
#ifdef _WIN32
    HANDLE p1_handle = CreateFile(p1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (p1_handle == INVALID_HANDLE_VALUE)
        ERROR("could not get time of %s", p1);

    FILETIME p1_time;
    if (!GetFileTime(p1_handle, NULL, NULL, &p1_time))
        ERROR("could not get time of %s", p1);

    CloseHandle(p1_handle);

    HANDLE p2_handle = CreateFile(p2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (p2_handle == INVALID_HANDLE_VALUE)
        ERROR("could not get time of %s", p2);

    FILETIME p2_time;
    if (!GetFileTime(p2_handle, NULL, NULL, &p2_time))
        ERROR("could not get time of %s", p2);

    CloseHandle(p2_handle);

    return CompareFileTime(&p1_time, &p2_time) == 1;
#else
    struct stat statbuf = {0};

    if (stat(p1, &statbuf) < 0)
        ERROR("could not get time of %s", p1);
    int p1_time = statbuf.st_mtime;

    if (stat(p2, &statbuf) < 0) 
        ERROR("could not get time of %s", p2);
    int p2_time = statbuf.st_mtime;

    return p1_time > p2_time;
#endif
}

bool exists(const char* path) {
    return access(path, F_OK) == 0;
}

char* own_path(void) {
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

bool is_outdated(char* path) {
    FOREACH(file, SOURCEFILES, {
        if (modifiedlater(file, path))
            return true;
    });
    return false;
}

// run
int run(strlist cmd) {
    if (cmd.count == 0)
        return 0;

    char* strcmd = join(" ", cmd);
    cmd = append(clone(cmd), NULL);

#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    INFO("%s", strcmd);
    if (!CreateProcessA(NULL, strcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        ERROR("task failed: %s", strcmd);
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (exit_code != 0) {
        ERROR("task failed: %s", strcmd);
        return exit_code;
    }
#else
    pid_t pid = fork();

    if (pid < 0) {
        ERROR("task failed: %s", strcmd);
    } else if (pid == 0) {
        INFO("%s", strcmd);
        if (execvp(cmd.items[0], (char* const*)cmd.items) < 0) {
            ERROR("task failed: %s", strcmd);
            return 1;
        }
    } else {
        int status;
        pid_t wpid = waitpid(pid, &status, 0);

        if (wpid == -1) {
            ERROR("task failed: %s", strcmd);
            return 1;
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                ERROR("task failed: %s", strcmd);
                return WEXITSTATUS(status);
            }
        }
        else {
            ERROR("task failed: %s", strcmd);
            return 1;
        }
    }
#endif
    
    return 0;
}

#define COMMAND_BUILD "build"
#define COMMAND_RUN "run"
#define COMMAND_TEST "test"
#define COMMAND_CLEAN "clean"

// commands
int ___run(strlist argv) {
    if (!exists(OUTPUT) || is_outdated(OUTPUT)) {
        if (config.process.build(argv) != 0)
            FATAL("cannot build executable");
    }

    return RUNL(config.process.passthrough, OUTPUT);
}

int ___build(strlist argv) {
    return CC(SOURCEFILES, OUTPUT);
}

#ifndef _CUILT_NO_MAIN
int main(int argc, const char* argv[]) {
    strlist _argv = { (size_t)(argc - 1), argv + 1 };
    config = merge_config(default_config(), ___config());
    const char* command = NULL;

    if (config.process.init)
        config.process.init(_argv);

    if (_argv.count == 0 || _argv.items[0][0] == '-')
        command = COMMAND_BUILD;
    else
        command = _argv.items[0];

    if (strcmp(command, COMMAND_RUN) == 0)
        config.log_level = LOG_FATAL;

    for (size_t i = 0; i < _argv.count; i++) {
        const char* arg = _argv.items[i];

        if (strcmp(arg, "-cc") == 0) {
            config.cc.command = _argv.items[++i];
        } else if (strcmp(arg, "-log") == 0) {
            arg = _argv.items[++i];
            if (strcmp(arg, "info") == 0)
                config.log_level = LOG_INFO;
            else if (strcmp(arg, "warn") == 0)
                config.log_level = LOG_WARN;
            else if (strcmp(arg, "error") == 0)
                config.log_level = LOG_ERROR;
            else if (strcmp(arg, "fatal") == 0)
                config.log_level = LOG_FATAL;
        } else if (strcmp(arg, "-cflags") == 0) {
            config.cc.flags = split(" ", _argv.items[++i]);
        } else if (strcmp(arg, "--") == 0) {
            strlist pst = {
                _argv.count - i - 1,
                _argv.items + i + 1,
            };
            config.process.passthrough = pst;
            break;
        }
    }

    if (modifiedlater(config.project.do_c, config.project.do_exe)) {
        INFO("rebuilding...");
        config.log_level = LOG_FATAL;
        if (RUN(config.cc.command, "-o", config.project.do_exe, config.project.do_c) != 0)
            FATAL("failed to rebuild");
        return RUNL(_argv, config.project.do_exe);
    }

    int res = 0;
    if (strcmp(command, COMMAND_BUILD) == 0 && config.process.build)
        res = config.process.build(_argv);
    else if (strcmp(command, COMMAND_RUN) == 0 && config.process.run)
        res = config.process.run(_argv);
    else if (strcmp(command, COMMAND_TEST) == 0 && config.process.test)
        res = config.process.test(_argv);
    else if (strcmp(command, COMMAND_CLEAN) == 0 && config.process.clean)
        res = config.process.clean(_argv);
    else
        FATAL("unknown command: %s", command);
    
    if (res != 0)
        FATAL("%s failed with exit code %d", command, res);

    return 0;
}
#endif // _CUILT_NO_MAIN

#endif // _CUILT_NO_IMPLEMENTATION

// end of cuilt.h
