# Tiny C Compiler - C Scripting Everywhere - The Smallest ANSI C compiler

[![armv7, aarch64, riscv64](https://github.com/zelang-dev/tinycc/actions/workflows/platforms.yml/badge.svg?branch=main)](https://github.com/zelang-dev/tinycc/actions/workflows/platforms.yml)

[![Windows, Linux, macOS](https://github.com/zelang-dev/tinycc/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/zelang-dev/tinycc/actions/workflows/cmake.yml)

- This fork adds [rpmalloc](https://github.com/zelang-dev/rpmalloc) and [cthread](https://github.com/zelang-dev/cthread) to  standard `libtcc1.a`, `libtcc.so/lib/dylib`, and **cross chains**.
- The `cthread` library adds `thrd_local` macro to emulate _tls_ **thread local storage** if needed, using normal usage behaviors as functions, the macro can be used the same even if `thread_local` is really available.
  - All thread related features to go through `pthread` even on Windows.
- added `dlopen` api for _Windows_, the **Dynamically Load** behavior works the same as _Linux_.
- added `cmake` build script, with `cross toolchain`, and a process to handle any `CMakeLists.txt` file.
- added downloadable platform packager installers, see [releases](https://github.com/zelang-dev/tinycc/releases)
- See [Changelog](Changelog)

## Features

- SMALL! You can compile and execute C code everywhere, for example on
  rescue disks.

- FAST! tcc generates optimized x86 code. No byte code
  overhead. Compile, assemble and link about 7 times faster than 'gcc
  -O0'.

- UNLIMITED! Any C dynamic library can be used directly. TCC is
  heading toward full ISOC99 compliance. TCC can of course compile
  itself.

- SAFE! tcc includes an optional memory and bound checker. Bound
  checked code can be mixed freely with standard code.

- Compile and execute C source directly. No linking or assembly
  necessary. Full C preprocessor included.

- C script supported : just add '#!/usr/local/bin/tcc -run' at the first
  line of your C source, and execute it directly from the command
  line.

## Documentation

### Installation uses `cmake` on a i386/x86_64/arm/aarch64/riscv64 - Windows/Linux/macOS/FreeBSD/NetBSD/OpenBSD hosts

#### To build compiler

_**Windows**_

```bash
mkdir build
cd build
cmake -D BUILD_TESTING=ON
cmake --build . --config Debug
ctest -C Debug --output-on-failure -F
cpack -G `NSIS/WIX/Nuget`
```

_**Linux**_

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -D BUILD_TESTING=ON ..
cmake --build .
ctest -C Debug --output-on-failure -F
cpack -G `DEB/RPM/DragNDrop`
```

### Use `tcc` on any `CMakeLists.txt` project

- To launch separate shell with path and custom environment: `tcc_prompt`
- To process **CMakeLists.txt**, and launch regular `tcc` executable, as cmake's compiler to use:
    `cmake_tcc Debug Native x86_64`
- You can pass two additional arguments to: `cmake_tcc`

> The command is an shortcut to:

    - On Windows
        `cmake .. -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE="%CMAKE_TOOLCHAIN_FILE%" -DCMAKE_BUILD_TYPE=%1 -DSYSTEM_NAME=%2 -DHOST_ARCH=%3 %4 %5`

    - Otherwise
        `cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE -DCMAKE_BUILD_TYPE=$1 -DSYSTEM_NAME=$2 -DHOST_ARCH=$3 $4 $5`

Then build as regular:
    `cmake --build .`

### Introduction

We assume here that you know ANSI C. Look at the example ex1.c to know
what the programs look like.

The include file <tcclib.h> can be used if you want a small basic libc
include support (especially useful for floppy disks). Of course, you
can also use standard headers, although they are slower to compile.

You can begin your C script with '#!/usr/local/bin/tcc -run' on the first
line and set its execute bits (chmod a+x your_script). Then, you can
launch the C code as a shell or perl script :-) The command line
arguments are put in 'argc' and 'argv' of the main functions, as in
ANSI C.

### Examples

ex1.c: simplest example (hello world). Can also be launched directly
as a script: './ex1.c'.

ex2.c: more complicated example: find a number with the four
operations given a list of numbers (benchmark).

ex3.c: compute fibonacci numbers (benchmark).

ex4.c: more complicated: X11 program. Very complicated test in fact
because standard headers are being used ! As for ex1.c, can also be launched
directly as a script: './ex4.c'.

ex5.c: 'hello world' with standard glibc headers.

tcc.c: TCC can of course compile itself. Used to check the code
generator.

tcctest.c: auto test for TCC which tests many subtle possible bugs. Used
when doing 'make test'.

### Full Documentation

Please read [tinycc-docs.html](tinycc-docs.html) to have all the features of TCC.

## License

TCC is distributed under **MIT** and the GNU Lesser General Public License (see COPYING file)
