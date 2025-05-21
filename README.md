# Cuilt

Cuilt is a build system for C that doesn't need installation. To see in-production use, check out [collaps](https://github.com/coddra/collaps).

Inspired by but not based on Tsoding's [nobuild](https://github.com/tsoding/nobuild).

**Cuilt is not yet crossplatform.** Works on linux for now.

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

Note, that `cuilt.c` contains the main function and other impementations too. Define macros `_CUILT_NO_MAIN` or `_CUILT_NO_IMPLEMENTATION` to disable them.

## Synopsis

```sh
./project [OPTIONS] [COMMAND] [EXTRA-ARGS]
```

Commands:
- `build` - build the project *- default*
- `run` - run the executable *- with `EXTRA-ARGS`, if specified*
- `test` - call the test function *- set `config.process.test` first (see below)*
- `deploy` - test the project than stage, commit, and push changes if test was successful. `join(" ", EXTRA-ARGS)` becomes the commit message
- `clean` - clean *- set `config.process.clean` first (see below)*

Options:
- `-cc <CC>` - override `config.cc.command` with `CC`
- `-cflags <CFLAGS>` - override `config.cc.flags` with `split(" ", CFLAGS)`
- `-log <LEVEL:debug|info|warn|error|fatal>` - override `config.log_level` with `LEVEL`
- `-release|-debug` - build in release or debug mode *- debug by default*
- `-force` - force build for each object file, even if it's up-to-date

## Configuration

Currently available config options are (with the default values):

```c
#include "cuilt.c"

CONFIG({
    .project = {
        .name = basename(cwd()), // name of the project and the output executable
                                 // the process will chdir to the containing directory
        .source = "src",         // source directory
        .bin = "bin",            // output directory
        .test = "test",          // test directory
    },
    .cc = {
        .command = "cc",
        .flags = LIST("-Wall", "-Wextra", "-Werror", "-std=c11"),
        .debug_flags = LIST("-g", "-O0"),
        .release_flags = LIST("-O3", "-dNDEBUG"),
        .pp = "cpp"              // preprocessor command - needed to determine file dependencies
    },
    .process = {                 // can be set to customize commands
        .init = NULL,            // will be called before any command
        .build = &__build,       // default build function
        .run = &__run,           // default run function
        .test = NULL,
        .deploy = &__deploy,     // default deploy function
        .clean = NULL,
    },
    .log_level = LOG_INFO,       // log level
})
```
