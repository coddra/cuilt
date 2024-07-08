# Cuilt

Cuilt is a corss-platform build system for C that does not need installation. To see in-production usage, check out [collaps](https://github.com/coddra/collaps).

Inspired by but not based on Tsoding's [nobuild](https://github.com/tsoding/nobuild).

**Disclaimer:** *Still under development. Partially tested on linux and windows, not tested on macos.*

## Usage

Place `cuilt.c` in your project's root directory alongside with a custom c file, for example `project.c`.
The most basic `project.c` would be:

```c
#include "cuilt.c"

CONFIG({  })
```

Then run to build your project:
```sh
cc project.c -o project
./project build
```

You don't have to build the `project` tool ever again, if you change the configuration, it will automatically rebuild itself.

Note, that `cuilt.c` contains the main function and other impementations too. Define `_CUILT_NO_MAIN` or `_CUILT_NO_IMPLEMENTATION` to disable them.

## Synopsis

```sh
./project [ARGS] [COMMAND] [PASS-THROUGH-ARGS]
```

Commands:
- `build` - build the project
- `run` - run the executable with the `PASS-THROUGH-ARGS`
- `test` - call the test function - you have to specify `config.process.test` first (see below)
- `clean` - clean - you have to specify `config.process.clean` first (see below)

Args:
- `-cc <CC>` - override `config.cc.command` with `CC`
- `-cflags <CFLAGS>` - override `config.cc.flags` with `split(" ", CFLAGS)`
- `-log <LEVEL:debug|info|warn|error|fatal>` - override `config.log_level` with `LEVEL`
- `-release|-debug` - build in release or debug mode (debug if default)
- `-force` - force build for each object file

## Configuration

Currently available config options are (with the default values):

```c
#include "cuilt.c"

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
        .debug_flags = LIST("-ggdb", "-O0"),
        .release_flags = LIST("-O3"),
    },
    .process = {                 // can be set to customize the processes
        .init = NULL,            // init function that will be called before any command
        // builds the executable using the set compiler and flags, 
        // from c files in the src directory and into the bin directory
        // with the set name
        .build = &__build,       // default build function
        // runs the executable with the PASS-THROUGH-ARGS
        .run = &__run,           // default run function
        .test = NULL,            // test function
        .clean = NULL,           // clean function
    },
    .log_level = LOG_INFO,       // log level
})
```
