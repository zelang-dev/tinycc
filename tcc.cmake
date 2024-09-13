
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

if(SYSTEM_NAME STREQUAL Native)
    include(TargetArch)
    target_architecture(TARGET_ARCH)
    list(LENGTH TARGET_ARCH cmake_target_arch_len)
    if (WIN32)
        set(SYSTEM_NAME Windows)
    elseif (APPLE)
        set(SYSTEM_NAME Darwin)
    elseif (UNIX)
        set(SYSTEM_NAME Linux)
    else()
        set(SYSTEM_NAME Generic)
    endif()
    set(HOST_ARCH ${TARGET_ARCH})
elseif(NOT HOST_ARCH AND NOT SYSTEM_NAME)
    message(SEND_ERROR "-D HOST_ARCH required to be specified:
i386, x86_64, i386-win32, x86_64-win32, x86_64-osx, arm, arm64, riscv64, arm-wince, arm64-osx

-D SYSTEM_NAME required to be specified:
Native, Windows, Linux, GNU, Generic, Android, FreeBSD, Darwin")
endif()

set(CMAKE_SYSTEM_NAME ${SYSTEM_NAME})
set(COMPILER_PREFIX ${HOST_ARCH})
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES HOST_ARCH)
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    if(HOST_ARCH STREQUAL arm)
        set(COMPILER_PREFIX ${HOST_ARCH}-wince)
    elseif(HOST_ARCH STREQUAL i386 OR HOST_ARCH STREQUAL x86_64)
        set(COMPILER_PREFIX ${HOST_ARCH}-win32)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    if(HOST_ARCH STREQUAL arm64 OR HOST_ARCH STREQUAL x86_64)
        set(COMPILER_PREFIX ${HOST_ARCH}-osx)
    endif()
endif()
find_program(CMAKE_C_COMPILER NAMES ${COMPILER_PREFIX}-tcc PATHS ENV PATH NO_DEFAULT_PATH)

set(TOOLCHAIN_PREFIX ${COMPILER_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(BUILD_SHARED_LIBS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -shared -w ")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -w ")
endif()

message (STATUS "Using compiler: ${CMAKE_C_COMPILER}")
