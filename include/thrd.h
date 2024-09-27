#ifndef _THRD_H_
#define _THRD_H_

#ifndef emulate_tls
#   define emulate_tls 1
#endif

#include "cthread.h"
#include "catomic.h"
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

thrd_static(uintptr_t, thrd, 0)

#endif /* _THRD_H_ */
