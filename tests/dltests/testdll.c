#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#include <stdio.h>

#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT int function2(void) {
    printf("Hello, world! from original library\n");
    return 0;
}

EXPORT int function(void) {
    printf("Hello, world!\n");
    return 0;
}
