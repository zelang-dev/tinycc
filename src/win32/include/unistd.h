/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 */
#ifndef _UNISTD_H
#define _UNISTD_H
#define __UNISTD_H_SOURCED__ 1
#if _WIN32
 /* This is intended as a drop-in replacement for unistd.h on Windows.
  * Please add functionality as neeeded.
  * https://stackoverflow.com/a/826027/1202830
  */

#include <stdlib.h>
#include <io.h>
#include "getopt.h" /* getopt at: https://gist.github.com/ashelly/7776712 */
#include <process.h> /* for getpid() and the exec..() family */
#include <direct.h> /* for _getcwd() and _chdir() */

#define srandom srand
#define random rand

  /* Values for the second argument to access.
     These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
     //#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#define access _access
#define dup2 _dup2
#define execve _execve
#define ftruncate _chsize
#define unlink _unlink
#define fileno _fileno
#define getcwd _getcwd
#define chdir _chdir
#define isatty _isatty
#define lseek _lseek
/* read, write, and close are NOT being #defined here, because while there are file handle specific versions for Windows, they probably don't work for sockets. You need to look at your app and consider whether to call e.g. closesocket(). */

#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
/* should be in some equivalent to <sys/types.h> */
typedef __int16           int16_t;
typedef __int32           int32_t;
typedef __int64           int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;

/* useconds_t is 32-bit on both LP32 and LP64. */
typedef uint32_t __useconds_t;
typedef __useconds_t useconds_t;
#else
#include <io.h>
#include <process.h>

#define __UNISTD_GETOPT__
#include <getopt.h>
#undef __UNISTD_GETOPT__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NO_ISOCEXT
#include <sys/types.h> /* For useconds_t. */

    int __cdecl __MINGW_NOTHROW usleep(useconds_t useconds);
#endif

#ifndef FTRUNCATE_DEFINED
#define FTRUNCATE_DEFINED
    /* This is defined as a real library function to allow autoconf
       to verify its existence. */
#if !defined(NO_OLDNAMES) || defined(_POSIX)
    int ftruncate(int, off_t);
#ifndef __CRT__NO_INLINE
    __CRT_INLINE int ftruncate(int __fd, off_t __length) {
        return _chsize(__fd, __length);
    }
#endif /* !__CRT__NO_INLINE */
#else
    int ftruncate(int, _off_t);
#ifndef __CRT__NO_INLINE
    __CRT_INLINE int ftruncate(int __fd, _off_t __length) {
        return _chsize(__fd, __length);
    }
#endif /* !__CRT__NO_INLINE */
#endif
#endif

#ifdef __cplusplus
}
#endif

#undef __UNISTD_H_SOURCED__
#endif
#endif /* _UNISTD_H */
