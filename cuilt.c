// cuilt.c
/*
Indie Developer License (IDL) 1.0

Copyright (c) 2024 András Nádas <andrew.reeds.mpg@gmail.com>

Permission is hereby granted, free of charge, to any individual or any group of
up to 12 individuals (collectively, "Users") or any company, organization, or
group of more than 12 individuals (collectively, "Entities") obtaining a copy
of this software and associated documentation files (the "Software"), to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, subject to the following conditions:

Users and Entities must provide proper credit, including the copyright holder's
name, a link to the original source, and an indication of any changes made to
the Software. This credit must be included in the software's documentation, in
the "About" section, or any other prominent location in the distribution.

Credit must not suggest the coptright holder endorses the Users, Entities, or
their use of the Software.

Entities are strictly prohibited from using the Software for commercial
purposes without obtaining a separate commercial license from the copyright
holder.

Entities desiring to use the Software for commercial purposes must contact the
copyright holder to negotiate terms and obtain a commercial license.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

By using the Software, Users and Entities agree to the terms of this license.
*/

#ifndef _CUILT_H
#define _CUILT_H

#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>


#ifndef __USE_XOPEN2K
#    define __USE_XOPEN2K
#endif
#include <dirent.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#define PATH_SEP "/"

#define HEAD(a, ...) a
#define TAIL(a, ...) __VA_ARGS__
#define FIRST(...) HEAD(__VA_ARGS__)
#define SECOND(...) FIRST(TAIL(__VA_ARGS__))
#define __CAT(a, b) a##b
#define CAT(a, b) __CAT(a, b)
#define __NOT0() 1, 1
#define __NOT() 1, 1
#define NOT(a) SECOND(CAT(__NOT, a)(), 0)
#define BOOL(a) NOT(NOT(a))
#define __IF_ELSE0(...) __ELSE0
#define __IF_ELSE1(...) __VA_ARGS__ __ELSE1
#define __ELSE0(...) __VA_ARGS__
#define __ELSE1(...)
#define IF_ELSE(condition) CAT(__IF_ELSE, BOOL(condition))
#define __HAS_ARGS() 0
#define HAS_ARGS(...) BOOL(HEAD(__HAS_ARGS __VA_ARGS__)())
#define __ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
    _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43,   \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N
#define ARG_COUNT(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(__ARG_COUNT(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, \
    46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, \
    18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))(0)
#define MK_VA(...) IF_ELSE(HAS_ARGS(__VA_ARGS__))(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)(0)

#define TERM_DEFAULT "\033[0m"
#define TERM_WHITE   "\033[1;37m"
#define TERM_CYAN    "\033[1;36m"
#define TERM_YELLOW  "\033[1;33m"
#define TERM_RED     "\033[1;31m"

typedef const char** strlist;

enum LOG_LVL {
    LOG_NULL = 0,
    LOG_DBG = 1,
    LOG_INF = 2,
    LOG_WRN = 3,
    LOG_ERR = 4,
    LOG_FTL = 5,
};

enum COMMAND {
    C_BUILD,
    C_RUN,
    C_TEST,
    C_DEPLOY,
    C_CLEAN
};

typedef int (*process_t)(strlist argv);

struct config_t {
    struct {
        const char* name;
        const char* source;
        const char* bin;
        const char* test;
    } project;
    struct {
        const char* command;
        strlist flags;
        strlist debug_flags;
        strlist release_flags;
        const char* pp;
    } cc;
    struct {
        process_t init;
        process_t build;
        process_t run;
        process_t test;
        process_t deploy;
        process_t clean;
    } process;
    enum LOG_LVL log_level;

    struct {
        const char* project_c;
        const char* project_exe;
        strlist extra_args;
        bool release;
        bool force;
    } __internal;
};
extern struct config_t config;
struct config_t default_config(void);
struct config_t __config(void);
struct config_t merge_config(struct config_t a, struct config_t b);
#define CONFIG(...) struct config_t __config(void) { \
    struct config_t res = __VA_ARGS__; \
    res.__internal.project_c = __FILE__; \
    return res; \
}

void msg(enum LOG_LVL level, const char* fmt, ...);
#define DEBUG(...) msg(LOG_DBG, __VA_ARGS__)
#define INFO(...) msg(LOG_INF, __VA_ARGS__)
#define WARN(...) msg(LOG_WRN, __VA_ARGS__)
#ifdef ERROR
#undef ERROR
#endif
#define ERROR(...) msg(LOG_ERR, __VA_ARGS__)
#define FATAL(...) msg(LOG_FTL, __VA_ARGS__)

#define BUFFER_SIZE 4096

char* reallocat(char* dest, const char* src);
char* enquote(const char* str);
char* argument(const char* str);

strlist mklist(size_t count, ...);
strlist* mklistlist(size_t count, ...);
size_t length(strlist list);
strlist append(strlist list, const char* item);
strlist concat(strlist list1, strlist list2);
strlist delete(strlist list, size_t index);
strlist split(const char* sep, const char* body);
char* join(const char* sep, strlist list);
inline static char* join_free(const char* sep, strlist list) {
    char* res = join(sep, list);
    free(list);
    return res;
}
#define LIST(...) mklist(MK_VA(__VA_ARGS__))
#define LIST_LIST(...) mklistlist(MK_VA(__VA_ARGS__))

#define PATH(...) join_free(PATH_SEP, LIST(__VA_ARGS__))

bool starts_with(const char* a, const char* b);
bool ends_with(const char* a, const char* b);
bool contains(const char* a, const char* b);

bool exists(const char* path);
bool modified_later(const char* p1, const char* p2);

strlist get_deps(const char* path);
bool is_outdated(const char *path, strlist deps);

strlist files_in(const char* dir);
strlist filter(strlist list, const char* ext);

char* read_file(const char* path);

char* own_path(void);
char* cwd(void);
const char* basename(const char* path);
char* no_extension(const char* path);

#define FILES(dir, ext) filter(files_in(dir), ext)
static inline void MKDIR(const char* path) {
    if (exists(path))
        return;
    if (mkdir(path, 0755) < 0)
        ERROR("failed to create %s", path);
    else
        DEBUG("created %s", path);
}
static inline void TOUCH(const char* path) {
    if (exists(path))
        return;
    if (open(path, O_CREAT | O_WRONLY, 0644) < 0)
        ERROR("failed to create %s", path);
    else
        DEBUG("created %s", path);
}
void mk_all_dirs(const char* path);
#define MKDIRS(path) mk_all_dirs(path)

int command(strlist* cmd, char** output);
int testcmd(strlist* cmd, const char* desired_output);
int testcmdf(strlist* cmd, const char* path);

#define CMDO(buffer, ...) command(LIST_LIST(LIST(__VA_ARGS__)), buffer)
#define CMDOL(buffer, args, ...) command(LIST_LIST(LIST(__VA_ARGS__), args), buffer)
#define CMD(...) CMDO(NULL, __VA_ARGS__)
#define CMDL(args, ...) CMDOL(NULL, args, __VA_ARGS__)

#define TEST(output, ...) testcmd(LIST_LIST(LIST(__VA_ARGS__)), output)
#define TESTF(path, ...) testcmdf(LIST_LIST(LIST(__VA_ARGS__)), path)

#define CC(files, output) command(LIST_LIST(LIST(config.cc.command, "-o", output), config.cc.flags, \
    (config.__internal.release ? config.cc.release_flags : config.cc.debug_flags), files), NULL)

extern strlist source;
extern const char* output;
#endif // _CUILT_H



#ifndef _CUILT_NO_IMPLEMENTATION

int __build(strlist argv);
int __run(strlist argv);
int __deploy(strlist argv);
struct config_t config;
strlist source;
const char* output;
struct config_t default_config(void) {
    struct config_t res = {
        .project = {
            .name = basename(cwd()),
            .source = "src",
            .bin = "bin",
            .test = "test",
        },
        .cc = {
            .command = "cc",
            .flags = LIST("-Wall", "-Werror", "-Wextra", "-std=c11"),
            .debug_flags = LIST("-g", "-O0"),
            .release_flags = LIST("-O3", "-DNDEBUG"),
            .pp = "cpp",
        },
        .process = {
            .init = NULL,
            .build = &__build,
            .run = &__run,
            .test = NULL,
            .deploy = &__deploy,
            .clean = NULL,
        },
        .log_level = LOG_INF,
        .__internal = {
            .extra_args = NULL,
            .project_c = NULL,
            .project_exe = own_path(),
            .release = false,
            .force = false,
        }
    };
    return res;
}

struct config_t merge_config(struct config_t a, struct config_t b) {
    struct config_t res = a;
#define MERGE(field) if (b.field) res.field = b.field
    MERGE(project.name);
    MERGE(project.source);
    MERGE(project.bin);
    MERGE(project.test);
    MERGE(cc.command);
    MERGE(cc.flags);
    MERGE(cc.debug_flags);
    MERGE(cc.release_flags);
    MERGE(cc.pp);
    MERGE(process.init);
    MERGE(process.build);
    MERGE(process.run);
    MERGE(process.test);
    MERGE(process.deploy);
    MERGE(process.clean);
    MERGE(log_level);
    MERGE(__internal.extra_args);
    MERGE(__internal.project_c);
    MERGE(__internal.project_exe);
#undef MERGE
    return res;
}

void msg(enum LOG_LVL level, const char* fmt, ...) {
    if (level < config.log_level)
        return;

    switch (level) {
#define CASE(level, color) case LOG_##level: fputs(isatty(fileno(stderr)) ? color "[" #level "]" TERM_DEFAULT " " : "[" #level "] ", stderr); break
        CASE(DBG, TERM_WHITE);
        CASE(INF, TERM_CYAN);
        CASE(WRN, TERM_YELLOW);
        CASE(ERR, TERM_RED);
        CASE(FTL, TERM_RED);
#undef CASE
        default: break;
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);

    if (level == LOG_FTL)
        exit(1);
}

char* reallocat(char* dest, const char* src) {
    size_t len = dest ? strlen(dest) : 0;
    dest = (char*)realloc(dest, len + strlen(src) + 1);
    dest[len] = '\0';
    strcat(dest, src);
    return dest;
}

char* enquote(const char* str) {
    size_t len = strlen(str);
    char* res = (char*)malloc(len + 3);
    memmove(res + 1, str, len + 1);
    res[0] = '"';
    res[len + 1] = '"';
    res[len + 2] = '\0';
    return res;
}

char* argument(const char* str) {
    if (*str == '\0' || contains(str, " ")) {
        return enquote(str);
    } else {
        char* res = (char*)malloc(strlen(str) + 1);
        strcpy(res, str);
        return res;
    }
}

strlist mklist(size_t count, ...) {
    strlist res = (strlist)malloc((count + 1) * sizeof(char*));

    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++)
        res[i] = va_arg(ap, char*);
    va_end(ap);

    res[count] = NULL;
    return res;
}
strlist* mklistlist(size_t count, ...) {
    strlist* res = (strlist*)malloc((count + 1) * sizeof(strlist));

    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++)
        res[i] = va_arg(ap, strlist);
    va_end(ap);

    res[count] = NULL;
    return res;
}

size_t length(strlist list) {
    if (list == NULL)
        return 0;

    size_t res = 0;
    while (list[res])
        res++;
    return res;
}

strlist append(strlist list, const char* item) {
    size_t len = length(list);
    list = (strlist)realloc(list, (len + 2) * sizeof(char*));
    list[len] = item;
    list[len + 1] = NULL;
    return list;
}

strlist concat(strlist list1, strlist list2) {
    size_t len = length(list1);
    size_t len2 = length(list2);
    list1 = (strlist)realloc(list1, (len + len2 + 1) * sizeof(char*));
    memmove(list1 + len, list2, len2 * sizeof(char*));
    list1[len + len2] = NULL;
    return list1;
}

strlist delete(strlist list, size_t index) {
    size_t len = length(list);
    memmove(list + index, list + index + 1, (len - index) * sizeof(char*));
    list[len - 1] = NULL;
    return list;
}

strlist split(const char* sep, const char* body) {
    strlist res = NULL;
    size_t len = strlen(body);
    char* tmp = (char*)malloc(len + 1);
    memcpy(tmp, body, len + 1);
    char* p = strtok(tmp, sep);
    while (p) {
        res = append(res, p);
        p = strtok(NULL, sep);
    }
    return res;
}

char* join(const char* sep, strlist list) {
    char* res;
    size_t len;
    if (list[0] == NULL) {
        res = (char*)malloc(sizeof(char));
        res[0] = '\0';
        return res;
    } else {
        len = strlen(list[0]);
        res = (char*)malloc((len + 1) * sizeof(char));
        memcpy(res, list[0], len);
    }
    size_t sep_len = strlen(sep);

    for (size_t i = 1; list[i] != NULL; i++) {
        size_t item_len = strlen(list[i]);
        res = (char*)realloc(res, (len + item_len + sep_len + 1) * sizeof(char));
        memcpy(res + len, sep, sep_len);
        len += sep_len;
        memcpy(res + len, list[i], item_len);
        len += item_len;
    }
    res[len] = '\0';
    return res;
}

bool starts_with(const char* a, const char* b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a, b, blen) == 0;
}

bool ends_with(const char* a, const char* b) {
    size_t alen = strlen(a);
    size_t blen = strlen(b);
    if (alen < blen)
        return false;
    return memcmp(a + alen - blen, b, blen) == 0;
}

bool contains(const char* a, const char* b) {
    return strstr(a, b) != NULL;
}

bool exists(const char* path) {
    return access(path, F_OK) == 0;
}

bool modified_later(const char* p1, const char* p2)
{
    struct stat statbuf = {0};

    if (stat(p1, &statbuf) < 0)
        ERROR("could not get time of %s", p1);
    int p1_time = statbuf.st_mtime;

    if (stat(p2, &statbuf) < 0)
        ERROR("could not get time of %s", p2);
    int p2_time = statbuf.st_mtime;

    return p1_time > p2_time;
}

strlist get_deps(const char* path) {
    char* buf = NULL;
    CMDO(&buf, config.cc.pp, "-MM", path);

    strlist res = split(" ", buf);
    free(buf);

    res++;
    for (int i = 0; res[i] != NULL; i++)
        if (res[i][0] == '\\')
            delete(res, i);

    size_t len = length(res);
    size_t lastlen = strlen(res[len - 1]);
    *(char*)(&res[len - 1][lastlen - 1]) = '\0';

    return res;
}

bool is_outdated(const char *path, strlist deps) {
    if (!exists(path))
        return true;

    for (int i = 0; deps[i] != NULL; i++) {
        if (modified_later(deps[i], path))
            return true;
    }
    return false;
}

strlist files_in(const char* dir) {
    strlist res = NULL;
    DIR* d = opendir(dir);
    if (d != NULL) {
        struct dirent* de;
        while ((de = readdir(d)) != NULL) {
            char* name = de->d_name;
            if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
                res = append(res, PATH(dir, name));
        }
        closedir(d);
    }
    return res;
}

strlist filter(strlist list, const char* ext) {
    int cut_start = -1;
    int cut_count = 1;
    int include_count = 0;
    size_t len = 0;
    for (size_t i = 0; list[i] != NULL; i++) {
        if (ends_with(list[i], ext)) {
            if (cut_start != -1)
                include_count++;
            else
                len++;
            continue;
        }

        if (cut_start == -1) {
            cut_start = i;
            continue;
        }

        if (include_count == 0) {
            cut_count++;
            continue;
        }

        memmove(list + cut_start, list + cut_start + cut_count, include_count * sizeof(char*));
        cut_start += include_count;
        cut_count++;
        len += include_count;
        include_count = 0;
    }
    memmove(list + cut_start, list + cut_start + cut_count, include_count * sizeof(char*));
    len += include_count;
    list = (strlist)realloc(list, (len + 1) * sizeof(char*));
    list[len] = NULL;
    return list;
}

char* read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        ERROR("failed to open file %s", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(length + 1);

    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);
    return content;
}

char* own_path(void) {
    char buffer[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
    if (len < 0)
        return NULL;
    char* result = (char*)malloc(len + 1);
    strcpy(result, buffer);
    return result;
}

char* parent(const char* path) {
    size_t len = strlen(path);
    char* res = (char*)malloc(len + 1);
    memcpy(res, path, len + 1);
    char* last = strrchr(res, PATH_SEP[0]);
    if (last == NULL)
        res[0] = '\0';
    else
        *last = '\0';
    return res;
}

char* cwd(void) {
    char* res = (char*)malloc(PATH_MAX);
    getcwd(res, PATH_MAX);
    return res;
}

const char* basename(const char* path) {
    const char* res = strrchr(path, '/');
    if (res == NULL)
        return path;
    return res + 1;
}

char* no_extension(const char* path) {
    const char* tmp = strrchr(path, '.');
    char* res = NULL;
    if (tmp == NULL) {
        size_t len = strlen(path);
        res = (char*)malloc(len + 1);
        memcpy(res, path, len + 1);
        return res;
    }
    size_t len = tmp - path;
    res = (char*)malloc(len + 1);
    memcpy(res, path, len);
    res[len] = '\0';
    return res;
}

void mk_all_dirs(const char *path) {
    size_t len = strlen(path);
    char* p = (char*)malloc(len + 1);
    memcpy(p, path, len + 1);

    char* next = strchr(p, PATH_SEP[0]);
    if (next == p)
        next = strchr(next + 1, PATH_SEP[0]);

    while (next != NULL) {
        *next = '\0';
        MKDIR(p);
        *next = PATH_SEP[0];
        next = strchr(++next, PATH_SEP[0]);
    }
    MKDIR(p);

    free(p);
    return;
}

int command(strlist* cmd, char** output) {
    char* strcmd = NULL;
    strlist listcmd = NULL;
    for (size_t i = 0; cmd[i] != NULL; i++) {
        for (size_t j = 0; cmd[i][j] != NULL; j++) {
            if (strcmd)
                strcmd = reallocat(strcmd, " ");
            strcmd = reallocat(strcmd, argument(cmd[i][j]));
        }
        if (!output)
            listcmd = concat(listcmd, cmd[i]);
    }

    DEBUG("running %s", strcmd);

	if (!output) {
        pid_t pid = fork();

#define FAIL { \
            ERROR("failed to run %s", strcmd); \
            free(strcmd); \
            free(listcmd); \
            return -1; \
        }
        if (pid < 0)
            FAIL;

        if (pid == 0) {
            execvp(listcmd[0], (char* const*)listcmd);
            FAIL;
        }

        free(strcmd);
        free(listcmd);
        int res;
        wait(&res);
        return res;
	}

    FILE *pipe = popen(strcmd, "r");
    if (!pipe) {
        ERROR("failed to run %s", strcmd);
        free(strcmd);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t content_size = BUFFER_SIZE;
    *output = (char*)malloc(content_size);

    size_t total_read = 0;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        size_t len = strlen(buffer);
        if (total_read + len + 1 >= content_size) {
            content_size *= 2;
            *output = (char*)realloc(*output, content_size);
        }
        strcpy(*output + total_read, buffer);
        total_read += len;
    }
    (*output)[total_read] = '\0';

    int res = pclose(pipe);
    if (res != 0)
        ERROR("%s exited with status %d", strcmd, res);

    free(strcmd);
    return res;
}

int testcmd(strlist* cmd, const char* desired_output) {
    char* output = NULL;
    int res = command(cmd, &output);
    DEBUG("output:\n%s", output);
    if (res == 0 && strcmp(output, desired_output) != 0)
        res = -1;
    free(output);
    return res;
}

int testcmdf(strlist* cmd, const char* path) {
    char* output = read_file(path);
    int res = testcmd(cmd, output);
    free(output);
    return res;
}

int __run(strlist argv) {
    return CMDL(config.__internal.extra_args, output);
}

int __build(strlist argv) {
    char* bin = parent(output);
    MKDIRS(bin);

    bool any_change = false;
    strlist objs = NULL;
    for (int i = 0; source[i] != NULL; i++) {
        char* obj = PATH(bin, reallocat(no_extension(basename(source[i])), ".o"));
        objs = append(objs, obj);

        if(!config.__internal.force && !is_outdated(obj, get_deps(source[i])))
            continue;
        any_change = true;

        int res = command(LIST_LIST(LIST(config.cc.command, source[i], "-c", "-o", obj),
            config.cc.flags, config.__internal.release ? config.cc.release_flags : config.cc.debug_flags),
            NULL);
        if (res != 0)
            FATAL("cannot compile %s", source[i]);
    }

    if (!any_change) {
        DEBUG("no changes made, skipping build");
        return 0;
    }

    int res = CC(objs, output);
    if (res == 0)
        INFO("build successful");
    else
        FATAL("build failed");

    return res;
}

int __deploy(strlist argv) {
    if (length(config.__internal.extra_args) == 0)
        FATAL("no commit message specified");

    if (config.process.test) {
        if (config.process.test(argv) != 0)
            FATAL("test failed, deployment canceled");
    }

    if (CMD("git", "add", "-A") != 0 ||
        CMD("git", "commit", "-m", join(" ", config.__internal.extra_args)) != 0 ||
        CMD("git", "push") != 0)
        FATAL("deployment failed");
    else
        INFO("deployment successful");
    return 0;
}

#ifndef _CUILT_NO_MAIN
int main(int argc, const char* argv[]) {
    chdir(parent(argv[0]));

    memmove(argv, argv + 1, (argc - 1) * sizeof(char*));
    argv[argc - 1] = NULL;

    config = merge_config(default_config(), __config());

    if (modified_later(config.__internal.project_c, config.__internal.project_exe)) {
        INFO("self rebuild...");
        config.log_level = LOG_FTL;
        if (CMD(config.cc.command, "-o", config.__internal.project_exe, config.__internal.project_c) != 0)
            FATAL("failed to rebuild %s", argv[0]);
        return CMDL(argv, config.__internal.project_exe);
    }

    if (config.process.init)
        config.process.init(argv);

    enum COMMAND command = C_BUILD;
    for (size_t i = 0; argv[i] != NULL; i++) {
#define OPTION(name, action) if (strcmp(argv[i], name) == 0) action
#define NEXT_ARG(option) if (argv[++i] == NULL) FATAL("missing argument for " option)
        OPTION("-cc", {
            NEXT_ARG("-cc");
            config.cc.command = argv[i];
        }) else OPTION("-log", {
            NEXT_ARG("-log");
            OPTION("debug", config.log_level = LOG_DBG);
            else OPTION("info", config.log_level = LOG_INF);
            else OPTION("warn", config.log_level = LOG_WRN);
            else OPTION("error", config.log_level = LOG_ERR);
            else OPTION("fatal", config.log_level = LOG_FTL);
            else ERROR("unknown log level: %s", argv[i]);
        }) else OPTION("-cflags", {
            NEXT_ARG("-cflags");
            config.cc.flags = split(" ", argv[i]);
        }) else OPTION("-debug", config.__internal.release = false);
        else OPTION("-release", config.__internal.release = true);
        else OPTION("-force", config.__internal.force = true);
        else if (argv[i][0] == '-') {
            ERROR("unknown option: %s", argv[i]);
        } else {
            OPTION("build", command = C_BUILD);
            else OPTION("run", command = C_RUN);
            else OPTION("test", command = C_TEST);
            else OPTION("deploy", command = C_DEPLOY);
            else OPTION("clean", command = C_CLEAN);
            else FATAL("unknown command: %s", argv[i]);

            config.__internal.extra_args = argv + i + 1;
            break;
        }
#undef OPTION
#undef NEXT_ARG
    }

    source = FILES(config.project.source, ".c");
    output = PATH(config.project.bin, config.__internal.release ? "release" : "debug", config.project.name);

    int res = 0;
    switch (command) {
#define SAFECALL(func) if (config.process.func == NULL) FATAL(#func " not implemented"); res = config.process.func(argv)
        case C_BUILD:
            SAFECALL(build);
            break;
        case C_RUN:
            config.log_level = LOG_FTL;
            if (config.process.build)
                config.process.build(argv);
            SAFECALL(run);
            break;
        case C_TEST:
            if (config.process.build)
                config.process.build(argv);
            SAFECALL(test);
            break;
        case C_DEPLOY:
            config.__internal.release = true;
            SAFECALL(deploy);
            break;
        case C_CLEAN:
            SAFECALL(clean);
            break;
#undef SAFECALL
    }

    return res;
}
#endif // _CUILT_NO_MAIN
#endif // _CUILT_NO_IMPLEMENTATION
#define _CUILT_NO_IMPLEMENTATION
// end of cuilt.c
