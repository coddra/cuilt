# Cuilt

Cuilt is not at all a build system for C.

Inspired by but not based on Tsoding's [nobuild](https://github.com/tsoding/nobuild).

**Disclaimer:** *Still under development. Partially tested on linux and windows, not tested on macos.*

## Usage

Place `cuilt.h` in your project's root directory alongside with a custom c file, for example `do.c`.
The most basic `do.c` would be:

```c
#include "cuilt.h"

CONFIG({0})
```

Then run to build your project:
```sh
cc do.c -o do
./do build
```

You don't have to build the `do` tool ever again, if you change the configuration, it will automatically rebuild itself.

Note, that `cuilt.h` contains the main function and other impementations too. Define `_CUILT_NO_MAIN` or `_CUILT_NO_IMPLEMENTATION` to disable them.

## Synopsis

```sh
./do [COMMAND] [ARGS...] [-- <PASS-THROUGH-ARGS...>]
```

Commands:
- `build` - build the project
- `run` - run the executable with the `PASS-THROUGH-ARGS`
- `test` - call the test function - you have to specify `config.process.test` first (see below)
- `clean` - clean - you have to specify `config.process.clean` first (see below)

Args:
- `-cc <CC>` - override `config.cc.command` with `CC`
- `-cflags <CFLAGS>` - override `config.cc.flags` with `split(" ", CFLAGS)`

## Configuration

Currently available config options are (with the default values):

```c
#include "cuilt.h"

CONFIG({
    .project = {
        .name = basename(cwd()), // name of the project and the output executable
        .src = "src",            // source directory
        .bin = "bin",            // build directory
        .test = "test",          // test directory
    },
    .cc = {
        .command = "cc",         // compiler command
        .flags = LIST("-Wall", "-Wextra", "-Werror", "-std=c11"), // compiler flags
    },
    .process = {                 // can be set to customize the processes
        .init = NULL,            // init function that will be called before any command
        .build = &___build,      // build function
        .run = &___run,          // run function
        .test = NULL,            // test function
        .clean = NULL,           // clean function
    },
    .log_level = LOG_INFO,       // log level
})

// in cuilt.h
int ___run(strlist argv) {
    if (!exists(OUTPUT) || is_outdated(OUTPUT)) {
        if (config.process.build(argv) != 0)
            FATAL("cannot build executable");
    }

    return RUNL(config.process.passthrough, OUTPUT);
}

int ___build(strlist argv) {
    // build the executable using the set compiler and flags, 
    // from c files in the src directory and into the bin directory
    // with the set name
    return CC(SOURCEFILES, OUTPUT);
}
```
