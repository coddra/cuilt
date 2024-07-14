# Cuilt

Cuilt is a corss-platform build system for C that does not need installation. To see in-production usage, check out [collaps](https://github.com/coddra/collaps).

Inspired by but not based on Tsoding's [nobuild](https://github.com/tsoding/nobuild).

**Disclaimer:** *Still under development. Mostly tested on linux, does not work on windows, not tested on macos. Stating it is cross-platform is a bit bold for now.*

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
- `build` - build the project *- default*
- `run` - run the executable with the `PASS-THROUGH-ARGS`
- `test` - call the test function *- set `config.process.test` first (see below)*
- `clean` - clean *- set `config.process.clean` first (see below)*

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
        .source = "src",
        .bin = "bin",
        .test = "test",
    },
    .cc = {
        .command = "cc",
        .flags = LIST("-Wall", "-Wextra", "-Werror", "-std=c11"),
        .debug_flags = LIST("-g", "-O0"),
        .release_flags = LIST("-O3"),
    },
    .process = {                 // can be set to customize commands
        .init = NULL,            // will be called before any command
        .build = &__build,       // default build function
        .run = &__run,           // default run function
        .test = NULL,
        .clean = NULL,
    },
    .log_level = LOG_INFO,       // log level
})
```
