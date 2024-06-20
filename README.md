# Cuilt

Cuilt is not at all a build system for C. Still under development. Partially tested on linux, not tested on windows nor macos.

Inspired by but not based on Tsoding's [nobuild](https://github.com/tsoding/nobuild).

## Usage

Place cuilt.h in your project's root directory alongside with a custom c file, for example `do.c`.
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

Note, that `cuilt.h` contains the main function and other impementations too. To use `cuilt.h` as a regular
header file, define `_CUILT_NO_IMPLEMENTATION` before including it.

Currently available config options are (with the default values):

```c
#include "cuilt.h"

CONFIG({
    .project = {
        .name = basename(cwd()), // name of the project and the output executable
        .src = "src",            // source directory
        .bin = "bin",            // build directory
        .test = "test",          // test directory
        .do_c = NULL,            // name of the custom c file. Will be set by the CONFIG macro
        .do_exe = own_path(),    // path to the 'do' executable
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
        .passthrough = { 0, NULL }, // passthrough arguments. Set after a double dash
    },
    .log_level = LOG_INFO,       // log level
})

// in cuilt.h
int ___build(strlist argv) {
    // build the executable using the set compiler and flags, 
    // from c files in the src directory and into the bin directory
    // with the set name
    return CC(SOURCEFILES, OUTPUT);
}

int ___run(strlist argv) {
    if (!exists(OUTPUT)) {
        int res = config.process.build(argv);
        if (res != 0)
            FATAL("cannot build executable");
    }

    return RUN(OUTPUT);
}
```
