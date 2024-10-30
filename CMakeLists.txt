cmake_minimum_required(VERSION 3.0...3.14)

project(
    tcc
    VERSION 0.9.29
    DESCRIPTION "C Scripting Everywhere - The Smallest ANSI C compiler."
    HOMEPAGE_URL "https://github.com/zelang-dev/tinycc/"
    LANGUAGES C
)

set(C_STANDARD 90)

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CheckCCompilerFlag)
include(GNUInstallDirs)
include(FetchContent)
include(CTest)

message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

set(_fmt TGZ)
if(WIN32)
  set(_fmt ZIP)
endif()

set(CPACK_GENERATOR ${_fmt})
SET(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
SET(CPACK_DEB_COMPONENT_INSTALL ON)
SET(CPACK_RPM_COMPONENT_INSTALL ON)
SET(CPACK_NUGET_COMPONENT_INSTALL ON)
SET(CPACK_WIX_COMPONENT_INSTALL ON)
SET(CPACK_NSIS_MODIFY_PATH ON)
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_INSTALL_CMAKE_CONFIGURATIONS Release)

set(OS_SYSTEM )
set(IS_REDHAT FALSE)
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P MATCHES "8")
        set(CMAKE_INSTALLER_DIR "C:/Program Files/")
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}")
    else()
        set(CMAKE_INSTALLER_DIR "C:/Program Files (x86)/")
        set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/${PROJECT_NAME}")
    endif()
elseif(NOT APPLE)
    file(STRINGS /etc/os-release OS_SYSTEM REGEX "^NAME=")
    string(REGEX REPLACE "NAME=\"(.*)\"" "\\1" OS_SYSTEM "${OS_SYSTEM}")
    if(${OS_SYSTEM} STREQUAL "CentOS Stream"
        OR ${OS_SYSTEM} STREQUAL "RedHat Linux"
        OR ${OS_SYSTEM} STREQUAL "Fedora"
        OR ${OS_SYSTEM} STREQUAL "CentOS Linux")
            set(IS_REDHAT TRUE)
    endif()
endif()

set(CPACK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_INSTALL_CONFIG_NAME ${CMAKE_BUILD_TYPE})

cmake_dependent_option(BUILD_TESTING
  "Build the unit tests when BUILD_TESTING is enabled and we are the root project" ON
  "BUILD_TESTING;CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
option(BUILD_SHARED_LIBS    "Build the library as a shared (dynamically-linked) " OFF)

include(TargetArch)

# Set target architectures
target_architecture(TARGET_ARCH)
list(LENGTH TARGET_ARCH cmake_target_arch_len)
message("Target architecture type: ${TARGET_ARCH}")

set(NATIVE_ARCH ${TARGET_ARCH})
set(DLLSUF .so)
set(LIBSUF .a)
if(WIN32)
    set(DLLSUF .dll)
    set(LIBSUF .lib)
    if(${TARGET_ARCH} STREQUAL armv7 OR ${TARGET_ARCH} STREQUAL arm)
        set(NATIVE_ARCH ${TARGET_ARCH}-wince)
    else()
        set(NATIVE_ARCH ${TARGET_ARCH}-win32)
    endif()
elseif(APPLE)
    set(DLLSUF .dylib)
    set(LIBSUF .dylib)
    set(NATIVE_ARCH ${TARGET_ARCH}-osx)
endif()

set(TCC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${TCC_DIR}/include)
set(SRC_DIR ${TCC_DIR}/src)
set(WIN_DIR ${SRC_DIR}/win32)
set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TCC_DIR}/built")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${TCC_DIR}/built")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Package")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY tcc)

enable_testing()
set( CORE_FILES
    ${SRC_DIR}/threads/rpmalloc.c
    ${SRC_DIR}/tcctools.c
    ${SRC_DIR}/libtcc.c
    ${SRC_DIR}/tccpp.c
    ${SRC_DIR}/tccgen.c
    ${SRC_DIR}/tccdbg.c
    ${SRC_DIR}/tccelf.c
    ${SRC_DIR}/tccasm.c
    ${SRC_DIR}/tccrun.c)

set( I386_FILES ${CORE_FILES}
    ${SRC_DIR}/i386-gen.c
    ${SRC_DIR}/i386-link.c
    ${SRC_DIR}/i386-asm.c )

set( I386_WIN32_FILES ${I386_FILES} ${SRC_DIR}/tccpe.c )
set( X86_64_FILES ${CORE_FILES}
    ${SRC_DIR}/x86_64-gen.c
    ${SRC_DIR}/x86_64-link.c
    ${SRC_DIR}/i386-asm.c )

set( X86_64_WIN32_FILES ${X86_64_FILES} ${SRC_DIR}/tccpe.c )
set( X86_64_OSX_FILES ${X86_64_FILES} ${SRC_DIR}/tccmacho.c )
set( ARM_FILES ${CORE_FILES}
    ${SRC_DIR}/arm-gen.c
    ${SRC_DIR}/arm-link.c
    ${SRC_DIR}/arm-asm.c )

set( ARM_WINCE_FILES ${ARM_FILES} ${SRC_DIR}/tccpe.c )
set( ARM64_FILES ${CORE_FILES} ${SRC_DIR}/arm64-gen.c ${SRC_DIR}/arm64-link.c ${SRC_DIR}/arm64-asm.c )
set( ARM64_WIN32_FILES ${ARM64_FILES} ${SRC_DIR}/tccpe.c )
set( ARM64_OSX_FILES ${ARM64_FILES} ${SRC_DIR}/tccmacho.c )
set( C67_FILES ${CORE_FILES} ${SRC_DIR}/c67-gen.c ${SRC_DIR}/c67-link.c ${SRC_DIR}/tcccoff.c )
set( RISCV64_FILES ${CORE_FILES} ${SRC_DIR}/riscv64-gen.c ${SRC_DIR}/riscv64-link.c ${SRC_DIR}/riscv64-asm.c )

# Set C compiler flags.
check_c_compiler_flag( -fno-strict-aliasing, HAVE_NOSTRICTALIASING )
if( HAVE_NOSTRICTALIASING )
  add_definitions( -fno-strict-aliasing )
endif( HAVE_NOSTRICTALIASING )

set (TCC_INCLUDE_DIRS -I${INC_DIR})
set (TCC_CROSS_INCLUDE_DIRS -I${WIN_DIR}/include -I${WIN_DIR}/include/winapi)
if (WIN32)
    list(APPEND TCC_INCLUDE_DIRS ${TCC_CROSS_INCLUDE_DIRS})
endif()

if(UNIX)
  add_definitions(-Wall -O2 -Wno-declaration-after-statement -Wno-pointer-sign -Wno-sign-compare -Wno-unused-result -Wno-unknown-pragmas)
  if(NOT APPLE)
    add_definitions( -Wno-format-truncation -Wno-stringop-truncation )
  elseif(APPLE)
    execute_process (
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE mac_include
    )
    string(STRIP ${mac_include} mac_include)
    set(USR_INCLUDE ${mac_include}/usr/include )
    message(STATUS "OSX include location: ${USR_INCLUDE}")
    list(APPEND TCC_INCLUDE_DIRS -I${USR_INCLUDE} )
    add_definitions(-fheinous-gnu-extensions -Wno-string-plus-int -Wno-deprecated-declarations -Wno-incompatible-pointer-types -DCONFIG_USR_INCLUDE=\"${USR_INCLUDE}\")
  endif()
endif()

if(UNIX)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
else()
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    if(CMAKE_C_SIMULATE_ID MATCHES "MSVC")
        add_definitions("/wd4244 /wd4267 /wd4033 /wd4715 /wd4996")
    endif()
endif()

set(TCC_VERSION ${PROJECT_VERSION})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(config.h.in ${SRC_DIR}/config.h)
if(WIN32)
    file(WRITE ${TCC_DIR}/tcc_env.bat)
    configure_file(cmake_env.bat.in ${TCC_DIR}/tcc_env.bat)
else()
    file(WRITE ${TCC_DIR}/tcc_env)
    configure_file(cmake_env.sh.in ${TCC_DIR}/tcc_env)
endif()
configure_file(config.texi.in config.texi)

include_directories(${CMAKE_BINARY_DIR})

set(CPACK_PACKAGE_VENDOR "https://github.com/zelang-dev/tinycc")
set(CPACK_PACKAGE_VERSION ${TCC_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${TCC_DIR}/COPYING.txt)
set(CPACK_RESOURCE_FILE_README ${TCC_DIR}/README.md)
set(CPACK_NSIS_MENU_LINKS "share/doc/tcc/tinycc-docs.html"
     "Tiny C Compiler Reference Documentation" "share/tcc_prompt.bat" "Tiny C Command Prompt")

set(CPACK_PACKAGE_CONTACT "lstubbs@zelang.dev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Lawrence Stubbs <${CPACK_PACKAGE_CONTACT}>")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
SET(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "C Scripting Everywhere - The Smallest ANSI C compiler")
set(CPACK_PACKAGE_DESCRIPTION ${CPACK_COMPONENT_LIBRARIES_DESCRIPTION})

set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_VENDOR})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${TARGET_ARCH})
include(CPack)

# Set target and sources
if( WIN32 AND NOT CYGWIN )
  if( ${TARGET_ARCH} STREQUAL i386 )
    set( NATIVE_TARGET TCC_TARGET_I386 )
    set( CODE_GENERATOR_SRCS ${I386_WIN32_FILES} )
  elseif( ${TARGET_ARCH} STREQUAL x86_64 )
    set( NATIVE_TARGET TCC_X86_64 )
    set( CODE_GENERATOR_SRCS ${X86_64_WIN32_FILES} )
  elseif( ${TARGET_ARCH} STREQUAL arm
    OR ${TARGET_ARCH} STREQUAL armv7
    OR ${TARGET_ARCH} STREQUAL arm64 )
        set( NATIVE_TARGET TCC_TARGET_ARM )
        set( CODE_GENERATOR_SRCS ${ARM_WINCE_FILES} )
  endif()
else()
  if( NOT APPLE AND ${TARGET_ARCH} STREQUAL riscv64 )
    set( NATIVE_TARGET TCC_TARGET_RISCV64 )
    set( CODE_GENERATOR_SRCS ${RISCV64_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm64 )
    set( NATIVE_TARGET TCC_TARGET_ARM64 )
    set( CODE_GENERATOR_SRCS ${ARM64_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL i386 )
    set( NATIVE_TARGET TCC_TARGET_I386 )
    set( CODE_GENERATOR_SRCS ${I386_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL x86_64 )
    set( NATIVE_TARGET TCC_X86_64 )
    set( CODE_GENERATOR_SRCS ${X86_64_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm_eabi )
    set( NATIVE_TARGET TCC_ARM_EABI )
    set( CODE_GENERATOR_SRCS ${ARM_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm )
    set( NATIVE_TARGET TCC_TARGET_ARM )
    set( CODE_GENERATOR_SRCS ${ARM_FILES} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL armv7 )
    set( NATIVE_TARGET TCC_TARGET_ARM )
    set( CODE_GENERATOR_SRCS ${ARM_FILES} )
  elseif( APPLE AND ${TARGET_ARCH} STREQUAL arm64 )
    set( NATIVE_TARGET "CONFIG_NO_CODESIGN")
    set( CODE_GENERATOR_SRCS ${ARM64_OSX_FILES} )
  elseif( APPLE AND ${TARGET_ARCH} STREQUAL x86_64 )
    set( NATIVE_TARGET "CONFIG_NO_CODESIGN")
    set( CODE_GENERATOR_SRCS ${X86_64_OSX_FILES} )
  endif()
endif( WIN32 AND NOT CYGWIN )

find_package(Threads)

################################################################################
# TCC compiler as library

if(BUILD_SHARED_LIBS)
    add_library(libtcc SHARED ${CODE_GENERATOR_SRCS})
    if(WIN32)
        target_compile_definitions(libtcc PUBLIC LIBTCC_AS_DLL)
    elseif(APPLE)
        target_compile_options(libtcc PUBLIC -shared -install_name libtcc.dylib -o libtcc.dylib)
    endif()
else()
    add_library(libtcc STATIC ${CODE_GENERATOR_SRCS})
    if(WIN32)
        target_compile_definitions(libtcc PUBLIC CONFIG_TCC_STATIC)
    else()
        target_compile_options(libtcc PUBLIC -static)
    endif()
endif()

target_compile_definitions(libtcc
    PUBLIC ONE_SOURCE=0
    PUBLIC "${NATIVE_TARGET}="
    PUBLIC TCC_LIBTCC1=\"libtcc1.a\"
    PUBLIC CONFIG_TCCDIR=\"${CMAKE_INSTALL_PREFIX}\"
    PUBLIC TCC_VERSION=\"${TCC_VERSION}\")

set(NATIVE_DEFINES )
if(${IS_REDHAT} STREQUAL TRUE)
    set(NATIVE_DEFINES -DCONFIG_LDDIR="\"lib64\"")
    target_compile_definitions(libtcc PUBLIC
        "TCC_TARGET_X86_64="
        CONFIG_LDDIR=\"lib64\")
endif()

if(WIN32)
    if( ${TARGET_ARCH} STREQUAL arm64 )
        target_compile_definitions(libtcc PUBLIC
            "TCC_TARGET_ARM="
            "TCC_ARM_VFP="
            "TCC_ARM_EABI="
            "TCC_ARM_HARDFLOAT="
            "TCC_TARGET_PE="
            CONFIG_TCC_LIBPATHS=\"{B}\")
    elseif( ${TARGET_ARCH} STREQUAL i386 )
        target_compile_definitions(libtcc PUBLIC
            "TCC_TARGET_I386="
            "TCC_TARGET_PE="
            CONFIG_TCC_LIBPATHS=\"{B}\")
    elseif( ${TARGET_ARCH} STREQUAL x86_64 )
        target_compile_definitions(libtcc PUBLIC
            "TCC_TARGET_X86_64="
            "TCC_TARGET_PE="
            CONFIG_TCC_LIBPATHS=\"{B}\")
    endif()
elseif (UNIX)
    if(APPLE)
        if( ${TARGET_ARCH} STREQUAL arm64 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_ARM64="
                "TCC_TARGET_MACHO=1")
        elseif( ${TARGET_ARCH} STREQUAL x86_64 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_X86_64="
                "TCC_TARGET_MACHO=1")
        endif()
    else()
        if( ${TARGET_ARCH} STREQUAL riscv64 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_RISCV64="
                CONFIG_TRIPLET=\"riscv64-linux-gnu\")
        elseif( ${TARGET_ARCH} STREQUAL arm64 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_ARM64="
                CONFIG_TRIPLET=\"aarch64-linux-gnu\")
        elseif( ${TARGET_ARCH} STREQUAL i386 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_I386="
                CONFIG_TRIPLET=\"i386-linux-gnu\")
        elseif( ${TARGET_ARCH} STREQUAL x86_64 AND NOT ${IS_REDHAT} STREQUAL TRUE)
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_X86_64="
                CONFIG_TRIPLET=\"x86_64-linux-gnu\")
        elseif( ${TARGET_ARCH} STREQUAL arm )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_ARM="
                "TCC_ARM_VFP="
                "TCC_ARM_EABI="
                "TCC_ARM_HARDFLOAT="
                CONFIG_TRIPLET=\"arm-linux-gnueabi\")
        elseif( ${TARGET_ARCH} STREQUAL armv7 )
            target_compile_definitions(libtcc PUBLIC
                "TCC_CPU_VERSION=7"
                "TCC_TARGET_ARM="
                "TCC_ARM_VFP="
                "TCC_ARM_EABI="
                "TCC_ARM_HARDFLOAT="
                CONFIG_TRIPLET=\"arm-linux-gnueabihf\")
        elseif( ${TARGET_ARCH} STREQUAL arm_eabi )
            target_compile_definitions(libtcc PUBLIC
                "TCC_TARGET_ARM="
                "TCC_ARM_VFP="
                "TCC_ARM_EABI="
                CONFIG_TRIPLET=\"arm-linux-gnueabi\")
        endif()
    endif()
    target_link_libraries(libtcc PUBLIC m dl Threads::Threads)
endif()

target_include_directories(libtcc
    PUBLIC ${SRC_DIR}
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/tcc-config)
set_target_properties(libtcc PROPERTIES
    PREFIX ""
    POSITION_INDEPENDENT_CODE ON)

################################################################################
# TCC compiler as executable

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    if(CMAKE_C_SIMULATE_ID MATCHES "MSVC")
        add_definitions(-W2 -MT -GS- -nologo)
    endif()
endif()

add_executable(tcc ${SRC_DIR}/tcc.c)
target_link_libraries(tcc PUBLIC libtcc)
install(TARGETS libtcc DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS tcc RUNTIME)

add_executable(c2str ${SRC_DIR}/conftest.c)
target_compile_definitions(c2str PUBLIC C2STR)
install(TARGETS c2str RUNTIME)

add_dependencies(c2str libtcc)
add_custom_command(
    OUTPUT ${SRC_DIR}/tccdefs_.h
    COMMAND c2str ${INC_DIR}/tccdefs.h tccdefs_.h)

################################################################################
# Runtime Library (libtcc1.a)

set( BCHECK_O ${SRC_DIR}/lib/bcheck.c )
set( B_O ${BCHECK_O} ${SRC_DIR}/lib/bt-exe.c ${SRC_DIR}/lib/bt-log.c ${SRC_DIR}/lib/bt-dll.c )
set( BT_O ${SRC_DIR}/lib/bt-exe.c ${SRC_DIR}/lib/bt-log.c ${SRC_DIR}/lib/runmain.c ${SRC_DIR}/lib/tcov.c )
set( EXTRA_O ${SRC_DIR}/lib/bt-dll.c ${SRC_DIR}/lib/bt-exe.c ${SRC_DIR}/lib/bt-log.c ${SRC_DIR}/lib/runmain.c ${BCHECK_O} )
set( CROSS_EXTRA_O ${BCHECK_O} ${SRC_DIR}/lib/bt-exe.c ${SRC_DIR}/lib/bt-log.c ${SRC_DIR}/lib/tcov.c )

set( I386_O
 ${SRC_DIR}/lib/libtcc1.c ${SRC_DIR}/lib/alloca.S
 ${SRC_DIR}/lib/alloca-bt.S ${SRC_DIR}/lib/stdatomic.c
 ${SRC_DIR}/lib/atomic.S ${SRC_DIR}/lib/builtin.c ${BT_O} )

set( X86_64_O
 ${SRC_DIR}/lib/libtcc1.c ${SRC_DIR}/lib/alloca.S
 ${SRC_DIR}/lib/alloca-bt.S ${SRC_DIR}/lib/stdatomic.c
 ${SRC_DIR}/lib/atomic.S ${SRC_DIR}/lib/builtin.c ${BT_O} )

set( ARM_O
 ${SRC_DIR}/lib/libtcc1.c ${SRC_DIR}/lib/armeabi.c
 ${SRC_DIR}/lib/alloca.S ${SRC_DIR}/lib/armflush.c
 ${SRC_DIR}/lib/stdatomic.c ${SRC_DIR}/lib/atomic.S
 ${SRC_DIR}/lib/builtin.c ${BT_O} )

set( ARM64_O
 ${SRC_DIR}/lib/lib-arm64.c ${SRC_DIR}/lib/stdatomic.c
 ${SRC_DIR}/lib/atomic.S ${SRC_DIR}/lib/builtin.c ${BT_O} )

set( RISCV64_O
 ${SRC_DIR}/lib/lib-arm64.c ${SRC_DIR}/lib/stdatomic.c
 ${SRC_DIR}/lib/atomic.S ${SRC_DIR}/lib/builtin.c ${BT_O} )

set( WIN_O
 ${SRC_DIR}/lib/libtcc1.c ${WIN_DIR}/lib/crt1.c
 ${WIN_DIR}/lib/crt1w.c ${WIN_DIR}/lib/wincrt1.c
 ${WIN_DIR}/lib/wincrt1w.c ${WIN_DIR}/lib/dllcrt1.c
 ${WIN_DIR}/lib/dllmain.c ${WIN_DIR}/lib/dlfcn.c )

set( DSO_O ${SRC_DIR}/lib/dsohandle.c )

set( OBJ-i386 ${I386_O} ${BCHECK_O} ${DSO_O} )
set( OBJ-x86_64 ${X86_64_O} ${SRC_DIR}/lib/va_list.c ${BCHECK_O} ${DSO_O} )
set( OBJ-x86_64-osx ${X86_64_O} ${SRC_DIR}/lib/va_list.c ${BCHECK_O} )
set( OBJ-i386-win32 ${I386_O} ${WIN_DIR}/lib/chkstk.S ${B_O} ${WIN_O} )
set( OBJ-x86_64-win32 ${X86_64_O} ${WIN_DIR}/lib/chkstk.S ${B_O} ${WIN_O} )
set( OBJ-arm64 ${ARM64_O} ${DSO_O} )
set( OBJ-arm64-osx ${ARM64_O} ${BCHECK_O} )
set( OBJ-arm ${ARM_O} ${DSO_O} )

set( OBJ-armv7 ${ARM_O} ${DSO_O} )
set( OBJ-arm-fpa ${ARM_O} ${DSO_O} )
set( OBJ-arm-fpa-ld ${ARM_O} ${DSO_O} )
set( OBJ-arm-vfp ${ARM_O} ${DSO_O} )
set( OBJ-arm-eabi ${ARM_O} ${DSO_O} )
set( OBJ-arm-eabihf ${ARM_O} ${DSO_O} )
set( OBJ-arm-wince ${ARM_O} ${WIN_O} )
set( OBJ-armv7-wince ${ARM_O} ${WIN_O} )
set( OBJ-riscv64 ${RISCV64_O} ${DSO_O} )

if (WIN32)
  if( ${TARGET_ARCH} STREQUAL i386 )
    set( LIB_SRCFILES ${OBJ-i386-win32} )
  elseif( ${TARGET_ARCH} STREQUAL x86_64 )
    set( LIB_SRCFILES ${OBJ-x86_64-win32} )
  elseif( ${TARGET_ARCH} STREQUAL arm )
    set( LIB_SRCFILES ${OBJ-arm-wince} )
  elseif( ${TARGET_ARCH} STREQUAL armv7 )
    set( LIB_SRCFILES ${OBJ-armv7-wince} )
  endif()
else()
  if( NOT APPLE AND ${TARGET_ARCH} STREQUAL riscv64 )
    set( LIB_SRCFILES ${OBJ-riscv64} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm64 )
    set( LIB_SRCFILES ${OBJ-arm64} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL i386 )
    set( LIB_SRCFILES ${OBJ-i386} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL x86_64 )
    set( LIB_SRCFILES ${OBJ-x86_64} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm_eabi )
    set( LIB_SRCFILES ${OBJ-arm-eabi} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm )
    set( LIB_SRCFILES ${OBJ-arm} )
  elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL armv7 )
    set( LIB_SRCFILES ${OBJ-armv7} )
  elseif( APPLE AND ${TARGET_ARCH} STREQUAL arm64 )
    set( LIB_SRCFILES ${OBJ-arm64-osx} )
  elseif( APPLE AND ${TARGET_ARCH} STREQUAL x86_64 )
    set( LIB_SRCFILES ${OBJ-x86_64-osx} )
  endif()
endif()

# compile C files to object files
list(REMOVE_ITEM LIB_SRCFILES ${EXTRA_O})
list(APPEND LIB_SRCFILES ${SRC_DIR}/threads/cmalloc.c)
file(MAKE_DIRECTORY ${LIB_DIR})
foreach(LIB_SRCFILE ${LIB_SRCFILES})
    get_filename_component(LIB_SRCFILE_NOEXT ${LIB_SRCFILE} NAME_WE)
    add_custom_command(
        OUTPUT ${LIB_DIR}/${LIB_SRCFILE_NOEXT}.o
        COMMAND tcc -Ox ${CFG_BUILD} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -D_REENTRANT ${TCC_INCLUDE_DIRS}
                    -L ${LIB_DIR}
                    -c ${LIB_SRCFILE}
                    -o ${LIB_DIR}/${LIB_SRCFILE_NOEXT}.o
        MAIN_DEPENDENCY ${LIB_SRCFILE}
        VERBATIM
        COMMENT "Compiling Runtime Library (${LIB_SRCFILE})")
    list(APPEND LIB_OBJFILES ${LIB_DIR}/${LIB_SRCFILE_NOEXT}.o)
endforeach()

if(UNIX)
    list(REMOVE_ITEM EXTRA_O ${SRC_DIR}/lib/bt-dll.c)
endif()

if(WIN32)
    add_custom_command(
        OUTPUT ${LIB_DIR}/pthread.o
        COMMAND tcc -Ox ${CFG_BUILD} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -DHAVE_CONFIG_H -D_REENTRANT -DPTW32_CLEANUP_C -DPTW32_STATIC_LIB ${TCC_INCLUDE_DIRS}
                    -L ${LIB_DIR}
                    -c ${SRC_DIR}/threads/deps/pthread-JMP.c
                    -o ${LIB_DIR}/pthread.o
        MAIN_DEPENDENCY ${SRC_DIR}/threads/deps/pthread-JMP.c
        VERBATIM
        COMMENT "Compiling Runtime Library (${SRC_DIR}/threads/deps/pthread-JMP.c)")
    list(APPEND LIB_OBJFILES ${LIB_DIR}/pthread.o)
endif()

add_custom_command(
    OUTPUT ${LIB_DIR}/cthread.o
    COMMAND tcc -Ox ${CFG_BUILD} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -DHAVE_CONFIG_H -D_REENTRANT -DPTW32_CLEANUP_C -DPTW32_STATIC_LIB ${TCC_INCLUDE_DIRS}
            -L ${LIB_DIR}
            -c ${SRC_DIR}/threads/cthread.c
            -o ${LIB_DIR}/cthread.o
    MAIN_DEPENDENCY ${SRC_DIR}/threads/cthread.c
    VERBATIM
    COMMENT "Compiling Runtime Library (${SRC_DIR}/threads/cthread.c)")
list(APPEND LIB_OBJFILES ${LIB_DIR}/cthread.o)


list(REMOVE_ITEM LIB_OBJFILES ${LIB_DIR}/tcctools.o)
# link all objects files to archive file
add_custom_target(
    libarchive ALL
    tcc -ar rcs ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc1.a ${LIB_OBJFILES}
    DEPENDS ${LIB_OBJFILES}
    BYPRODUCTS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc1.a
    COMMENT "Linking Runtime Library"
    SOURCES ${LIB_SRCFILES})
add_dependencies(libarchive libtcc)
install(FILES ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc1.a DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(WIN32 OR APPLE)
    add_custom_target(
        libtcc_o ALL
        tcc -Ox ${CFG_BUILD} ${BFLAGS} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable ${TCC_INCLUDE_DIRS} -c ${SRC_DIR}/libtcc.c -o ${LIB_DIR}/libtcc.o
        DEPENDS libarchive
        VERBATIM
        COMMENT "Compiling (${SRC_DIR}/libtcc.c)"
        SOURCES ${SRC_DIR}/libtcc.c )

    add_custom_target(
        libtcc_a ALL
        tcc -ar rcs ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc${LIBSUF} ${LIB_DIR}/libtcc.o
        DEPENDS libtcc_o
        BYPRODUCTS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc${LIBSUF}
        VERBATIM
        COMMENT "Linking Archive ${LIB_DIR}/libtcc.o" )
    install(FILES ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc${LIBSUF} DESTINATION ${CMAKE_INSTALL_LIBDIR})

    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(
            OUTPUT libtcc.def
            COMMAND tcc -impdef libtcc -o libtcc.def
            MAIN_DEPENDENCY ${LIB_OBJFILES}
            VERBATIM
            COMMENT "Create Windows Import DLL Library (${LIB_DIR}/libtcc.def)")
        install(FILES libtcc.def DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()
endif()

foreach(EXTRA_SRCFILE ${EXTRA_O})
    get_filename_component(EXTRA_SRCFILE_NOEXT ${EXTRA_SRCFILE} NAME_WE)
    set(BFLAGS )
    if(${EXTRA_SRCFILE} STREQUAL ${BCHECK_O} )
        set(BFLAGS -bt)
    endif()
    add_custom_target(
        ${EXTRA_SRCFILE_NOEXT} ALL
        tcc -Ox ${CFG_BUILD} ${BFLAGS} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -D_REENTRANT ${TCC_INCLUDE_DIRS} -c ${EXTRA_SRCFILE} -o ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${EXTRA_SRCFILE_NOEXT}.o
        VERBATIM
        COMMENT "Compiling Extra Runtime Library (${EXTRA_SRCFILE})"
        SOURCES ${EXTRA_SRCFILE})
    install(FILES ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${EXTRA_SRCFILE_NOEXT}.o DESTINATION ${CMAKE_INSTALL_LIBDIR})
endforeach()

# cross compiler targets and cross libtcc1.a to build
if(${TARGET_ARCH} STREQUAL x86_64)
    set(TCC_X i386 i386-win32 x86_64-win32 x86_64 x86_64-osx armv7 arm64 riscv64 armv7-wince arm64-osx)
elseif(${TARGET_ARCH} STREQUAL i386 OR ${TARGET_ARCH} STREQUAL arm)
    set(TCC_X i386 i386-win32 arm arm-wince)
else()
    set(TCC_X i386 i386-win32 x86_64-win32 x86_64 x86_64-osx armv7 arm64 riscv64 armv7-wince arm64-osx)
endif()

set(CROSS_TARGET_i386 TCC_TARGET_I386
 CONFIG_TCC_CROSSPREFIX=\"i386-\"
 CONFIG_TCC_CRTPREFIX=\"/usr/i686-linux-gnu/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/i686-linux-gnu/lib:/usr/lib/i386-linux-gnu\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/i686-linux-gnu/include:/usr/include\")

set(CROSS_TARGET_x86_64 TCC_TARGET_X86_64
 CONFIG_TRIPLET=\"x86_64-linux-gnu\"
 CONFIG_TCC_CROSSPREFIX=\"x86_64-\")

set(CROSS_TARGET_i386-win32 TCC_TARGET_I386 TCC_TARGET_PE
 CONFIG_TCC_CROSSPREFIX=\"i386-win32-\"
 CONFIG_TCCDIR=\"${CMAKE_INSTALL_PREFIX}\")

set(CROSS_TARGET_x86_64-win32 TCC_TARGET_X86_64 TCC_TARGET_PE
 CONFIG_TCC_CROSSPREFIX=\"x86_64-win32-\"
 CONFIG_TCCDIR=\"${CMAKE_INSTALL_PREFIX}\")

set(CROSS_TARGET_arm-wince TCC_TARGET_ARM TCC_ARM_VFP TCC_ARM_EABI TCC_ARM_HARDFLOAT TCC_TARGET_PE
 CONFIG_TCC_CROSSPREFIX=\"arm-wince-\"
 CONFIG_TCCDIR=\"${CMAKE_INSTALL_PREFIX}\")

set(CROSS_TARGET_arm TCC_TARGET_ARM TCC_ARM_VFP TCC_ARM_EABI TCC_ARM_HARDFLOAT
 CONFIG_TCC_CROSSPREFIX=\"arm-\"
 CONFIG_TCC_CRTPREFIX=\"/usr/arm-linux-gnueabi/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/arm-linux-gnueabi/lib:/usr/lib/arm-linux-gnueabi\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/arm-linux-gnueablude/include:/usr/include\")

set(CROSS_TARGET_armv7-wince TCC_TARGET_ARM TCC_ARM_VFP TCC_ARM_EABI TCC_ARM_HARDFLOAT TCC_TARGET_PE
 CONFIG_TCC_CROSSPREFIX=\"armv7-wince-\"
 CONFIG_TCCDIR=\"${CMAKE_INSTALL_PREFIX}\")

set(CROSS_TARGET_armv7 TCC_TARGET_ARM TCC_ARM_VFP TCC_ARM_EABI TCC_ARM_HARDFLOAT
 CONFIG_TCC_CROSSPREFIX=\"armv7-\"
 CONFIG_TCC_CRTPREFIX=\"/usr/arm-linux-gnueabihf/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/arm-linux-gnueabihf/lib:/usr/lib/arm-linux-gnueabihf\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/arm-linux-gnueabihf/include:/usr/include\")

set(CROSS_TARGET_arm64 TCC_TARGET_ARM64
 CONFIG_TCC_CROSSPREFIX=\"arm64-\"
  CONFIG_TCC_CRTPREFIX=\"/usr/aarch64-linux-gnu/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/aarch64-linux-gnu/lib:/usr/lib/aarch64-linux-gnu\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/aarch64-linux-gnu/include:/usr/include\")

set(CROSS_TARGET_riscv64 TCC_TARGET_RISCV64
 CONFIG_TCC_CROSSPREFIX=\"riscv64-\"
 CONFIG_TCC_CRTPREFIX=\"/usr/riscv64-linux-gnu/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/riscv64-linux-gnu/lib:/usr/lib/riscv64-linux-gnu\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/riscv64-linux-gnu/include:/usr/include\")

set(CROSS_TARGET_x86_64-osx TCC_TARGET_X86_64 TCC_TARGET_MACHO
 CONFIG_TCC_CROSSPREFIX=\"x86_64-osx-\"
 CONFIG_TCC_LIBPATHS=\"{B}/lib\")

set(CROSS_TARGET_arm64-osx TCC_TARGET_ARM64 TCC_TARGET_MACHO
 CONFIG_TRIPLET=\"aarch64-linux-gnu\"
 CONFIG_TCC_CROSSPREFIX=\"arm64-osx-\"
 CONFIG_TCC_CRTPREFIX=\"/usr/aarch64-linux-gnu/lib\"
 CONFIG_TCC_LIBPATHS=\"{B}:/usr/aarch64-linux-gnu/lib:/usr/lib/aarch64-linux-gnu\"
 CONFIG_TCC_SYSINCLUDEPATHS=\"{B}/include:/usr/aarch64-linux-gnu/include:/usr/include\")

set(CROSS_DEFINES -Wall -O2 -Wno-declaration-after-statement -fno-strict-aliasing
 -Wno-pointer-sign -Wno-sign-compare -Wno-unused-result -Wno-format-truncation -Wno-stringopcation)

foreach(X ${TCC_X})
    set(tcc_cross ${X}-tcc)
    set(CROSS_SRCFILES ${OBJ-${X}})
    set(CROSS_TARGET ${CROSS_TARGET_${X}})
    set(CROSS_INCLUDE ${INC_DIR})
    if(${X} STREQUAL i386-win32
        OR ${X} STREQUAL x86_64-win32
        OR ${X} STREQUAL arm-wince
        OR ${X} STREQUAL armv7-wince)
            set(CROSS_INCLUDE ${INC_DIR} -I${WIN_DIR}/include -I${WIN_DIR}/include/winapi)
    endif()
    add_executable(${tcc_cross} ${SRC_DIR}/tcc.c)
    target_compile_definitions(${tcc_cross} PUBLIC ${CROSS_TARGET})
    add_dependencies(${tcc_cross} c2str)
    if(UNIX)
        target_link_libraries(${tcc_cross} PUBLIC m dl Threads::Threads)
    endif()
    install(TARGETS ${tcc_cross} RUNTIME)

    if(WIN32 AND
        (NOT ${X} STREQUAL arm-wince OR NOT ${X} STREQUAL armv7-wince OR NOT ${X} STREQUAL i386-win32 OR NOT ${X} STREQUAL x86_64-win32))
            list(REMOVE_ITEM CROSS_SRCFILES ${B_O})
            list(REMOVE_ITEM CROSS_SRCFILES ${SRC_DIR}/lib/tcov.c )
    endif()

    if(UNIX AND
        (${X} STREQUAL x86_64-osx OR ${X} STREQUAL arm OR ${X} STREQUAL armv7 OR ${X} STREQUAL arm64 OR ${X} STREQUAL riscv64 OR ${X} STREQUAL arm64-osx OR ${X} STREQUAL i386 ))
            list(REMOVE_ITEM CROSS_SRCFILES ${CROSS_EXTRA_O})
    endif()

    if(APPLE AND
        (${X} STREQUAL x86_64 ))
            list(REMOVE_ITEM CROSS_SRCFILES ${CROSS_EXTRA_O})
    endif()

    if((${NATIVE_ARCH} STREQUAL riscv64 OR ${NATIVE_ARCH} STREQUAL armv7 OR ${NATIVE_ARCH} STREQUAL arm OR ${NATIVE_ARCH} STREQUAL arm64)
        AND (${X} STREQUAL i386 OR ${X} STREQUAL x86_64))
            list(REMOVE_ITEM CROSS_SRCFILES ${CROSS_EXTRA_O})
    endif()

    # compile C files to object files
    file(MAKE_DIRECTORY ${LIB_DIR})
    foreach(CROSS_SRCFILE ${CROSS_SRCFILES})
        get_filename_component(CROSS_SRCFILE_NOEXT ${CROSS_SRCFILE} NAME_WE)
        add_custom_command(
            OUTPUT ${LIB_DIR}/${X}-${CROSS_SRCFILE_NOEXT}.o
            COMMAND ${tcc_cross} -w ${CROSS_DEFINES} -D_REENTRANT
                -L ${LIB_DIR}
                -c ${CROSS_SRCFILE}
                -o ${LIB_DIR}/${X}-${CROSS_SRCFILE_NOEXT}.o
                -I ${CROSS_INCLUDE}
            MAIN_DEPENDENCY ${CROSS_SRCFILE}
            DEPENDS ${tcc_cross}
            VERBATIM
            COMMENT "Cross Compiling Runtime Library (${CROSS_SRCFILE}) for ${X}")
        list(APPEND CROSS_OBJFILES ${LIB_DIR}/${X}-${CROSS_SRCFILE_NOEXT}.o)
        add_dependencies(${tcc_cross} libarchive)
    endforeach()

    if(${X} STREQUAL i386-win32
        OR ${X} STREQUAL x86_64-win32
        OR ${X} STREQUAL arm-wince
        OR ${X} STREQUAL armv7-wince)
            add_custom_command(
                OUTPUT ${LIB_DIR}/${X}_pthread.o
                COMMAND ${tcc_cross} -s -w -Ox -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -DHAVE_CONFIG_H -D_REENTRANT -DPTW32_CLEANUP_C -DPTW32_STATIC_LIB
                    -L ${LIB_DIR}
                    -c ${SRC_DIR}/threads/deps/pthread-JMP.c
                    -o ${LIB_DIR}/${X}_pthread.o
                    -I ${CROSS_INCLUDE}
                MAIN_DEPENDENCY ${SRC_DIR}/threads/deps/pthread-JMP.c
                DEPENDS ${tcc_cross}
                VERBATIM
                COMMENT "Cross Compiling Runtime Library (${SRC_DIR}/threads/deps/pthread-JMP.c)")
            list(APPEND CROSS_OBJFILES ${LIB_DIR}/${X}_pthread.o)
            add_custom_command(
                OUTPUT ${LIB_DIR}/${X}_cthread.o
                COMMAND ${tcc_cross} -Ox ${CFG_BUILD} -fPIC -fno-omit-frame-pointer -Wno-unused-function -Wno-unused-variable -D_REENTRANT -DPTW32_CLEANUP_C -DPTW32_STATIC_LIB -DHAVE_CONFIG_H
                        -L ${LIB_DIR}
                        -c ${SRC_DIR}/threads/cthread.c
                        -o ${LIB_DIR}/${X}_cthread.o
                        -I ${CROSS_INCLUDE}
                MAIN_DEPENDENCY ${SRC_DIR}/threads/cthread.c
                DEPENDS ${tcc_cross}
                VERBATIM
                COMMENT "Cross Compiling Runtime Library (${SRC_DIR}/threads/cthread.c)")
            list(APPEND CROSS_OBJFILES ${LIB_DIR}/${X}_cthread.o)
    endif()

    # link all objects files to archive file
    add_custom_target(
        ${X}_libarchive ALL
        ${tcc_cross} -ar rcs ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X}-libtcc1.a ${CROSS_OBJFILES}
        DEPENDS ${CROSS_OBJFILES}
        BYPRODUCTS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X}-libtcc1.a
        COMMENT "Cross Linking Archive Runtime Library - ${X}" )
    add_dependencies(${tcc_cross} tcc)
    install(FILES ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${X}-libtcc1.a DESTINATION ${CMAKE_INSTALL_LIBDIR})
    set(CROSS_OBJFILES "")
endforeach()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

file(GLOB WIN32_DEFS ${WIN_DIR}/lib/*.def)
if(WIN32)
    install(DIRECTORY ${WIN_DIR}/include/ DESTINATION include)
    install(DIRECTORY include/ DESTINATION include)
    install(DIRECTORY include/ DESTINATION lib/include)
    install(FILES ${WIN32_DEFS} DESTINATION lib)
    install(FILES ${SRC_DIR}/tcclib.h DESTINATION lib/include)
    install(FILES ${SRC_DIR}/tcclib.h DESTINATION include)
    install(FILES ${SRC_DIR}/libtcc.h DESTINATION lib)
    install(FILES tcc_prompt.bat DESTINATION ${CMAKE_INSTALL_DATADIR})
    install(FILES cmake_tcc.bat tcc_env.bat DESTINATION bin)
else()
    install(DIRECTORY ${WIN_DIR}/include/ DESTINATION lib/tcc/win32/include)
    install(DIRECTORY include/ DESTINATION lib/tcc/include)
    install(FILES ${WIN32_DEFS} DESTINATION lib/tcc/win32/lib)
    install(FILES ${SRC_DIR}/tcclib.h DESTINATION lib/tcc/win32/include)
    install(FILES ${SRC_DIR}/tcclib.h DESTINATION lib/tcc/include)
    install(FILES ${SRC_DIR}/libtcc.h DESTINATION include)
    install(FILES cmake_tcc tcc_env tcc_prompt
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
        DESTINATION bin )
endif()

install(FILES tcc.cmake DESTINATION share)
install(FILES tinycc-docs.html DESTINATION ${CMAKE_INSTALL_DOCDIR})
