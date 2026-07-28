#ifdef DEF_LIBRARY_DLL

__declspec(dllexport) int def_library_value(void)
{
    return 42;
}

#else

#include <stdio.h>

int def_library_value(void);

int main(void)
{
    int value = def_library_value();
    printf("value: %d\n", value);
    return value != 42;
}

#endif
