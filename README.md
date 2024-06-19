# Cuilt

Cuilt is not at all a build system for C.

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

Note, that `cuilt.h` contains the main function and other impementations too. To use `cuilt.h` as a regular
header file, define `_CUILT_NO_IMPLEMENTATION` before including it.

Currently available config options are (with the default values):

```c
#include "cuilt.h"

CONFIG({
    .project = {
        .name = basename(cwd()),    // name of the project and the output executable
        .src_d = "src",             // source directory
        .bin_d = "bin",             // build directory
        .test_d = "test",           // test directory
        .do_c = NULL,               // name of the custom c file. Will be filled by the CONFIG macro
        .build_exe = own_path(),    // path to the 'do' executable
    },
    .cc = {
        .command = "cc",            // compiler command
        .flags = MKLIST("-Wall", "-Wextra", "-Werror", "-std=c11"), // compiler flags
    },
    .process = {                    // can be set to customize the processes
        .init = NULL,               // init function that will be called before any command
        .build = &___build,         // build function (default builds from src to bin with all c files)
        .run = &___run,             // run function
        .test = NULL,               // test function
        .clean = NULL,              // clean function
    },
    .log_level = LOG_INFO,          // log level
})
```
