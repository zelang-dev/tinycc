
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
if(SYSTEM_NAME STREQUAL Native AND HOST_ARCH)
    if (WIN32)
        set(SYSTEM_NAME Windows)
    elseif (APPLE)
        set(SYSTEM_NAME Darwin)
    elseif (UNIX)
        set(SYSTEM_NAME Linux)
    else()
        set(SYSTEM_NAME Generic)
    endif()
    set(IS_NATIVE TRUE)
elseif(NOT HOST_ARCH OR NOT SYSTEM_NAME OR NOT CMAKE_BUILD_TYPE)
    message(SEND_ERROR "the `cmake_tcc . . .` missing:
- `BUILD_TYPE` required to be specified: Debug, Release, ...
- `SYSTEM_NAME` required to be specified: Native, Windows, Linux, GNU, Generic, Android, FreeBSD, Darwin, ...
- `HOST_ARCH` required to be specified: i386, x86_64, armv7, arm64, riscv64, ...

Note: Native is for regular `tcc` compiler, otherwise cross compiler.
")
endif()

set(CMAKE_SYSTEM_NAME ${SYSTEM_NAME})
set(COMPILER_PREFIX ${HOST_ARCH})
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES HOST_ARCH)
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    if(HOST_ARCH STREQUAL armv7)
        set(COMPILER_PREFIX ${HOST_ARCH}-wince)
    elseif(HOST_ARCH STREQUAL i386 OR HOST_ARCH STREQUAL x86_64)
        set(COMPILER_PREFIX ${HOST_ARCH}-win32)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    if(HOST_ARCH STREQUAL arm64 OR HOST_ARCH STREQUAL x86_64)
        set(COMPILER_PREFIX ${HOST_ARCH}-osx)
    endif()
endif()

set(EXT )
if(WIN32)
    set(EXT .exe)
endif()

if(IS_NATIVE STREQUAL TRUE)
    set(program "$ENV{CC}")
else()
    set(program "$ENV{CPATH}/${COMPILER_PREFIX}-tcc${EXT}")
endif()

find_program(CMAKE_C_COMPILER NAMES ${program} PATHS ENV PATH NO_DEFAULT_PATH)

set(TOOLCHAIN_PREFIX ${COMPILER_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(IS_NATIVE STREQUAL TRUE)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -bt -b -g ")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g ")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -L\"$ENV{LIBRARY_PATH}\" -I\"$ENV{C_INCLUDE_PATH}\" ")

message (STATUS "Using compiler: ${CMAKE_C_COMPILER}")
message (STATUS "Using architecture: ${HOST_ARCH}")
message (STATUS "Using library directory: $ENV{LIBRARY_PATH}")
message (STATUS "Using toolchain prefix: ${TOOLCHAIN_PREFIX}")
