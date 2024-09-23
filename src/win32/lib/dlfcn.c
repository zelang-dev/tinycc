#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dlfcn.h>

static struct {
    long err_code;
    const char *err_runtime;
} var = {
    0,
    NULL
};

void *dlopen(const char *filename, int flags) {
    HINSTANCE hInst;

    hInst = LoadLibrary(filename);
    if (hInst == NULL) {
        var.err_code = GetLastError();
        var.err_runtime = "dlopen";
    }
    return hInst;
}

int dlclose(void *handle) {
    BOOL ok;
    int rc = 0;

    ok = FreeLibrary((HINSTANCE)handle);
    if (!ok) {
        var.err_code = GetLastError();
        var.err_runtime = "dlclose";
        rc = -1;
    }
    return rc;
}

void *dlsym(void *handle, const char *name) {
    FARPROC fp;

    fp = GetProcAddress((HINSTANCE)handle, name);
    if (!fp) {
        var.err_code = GetLastError();
        var.err_runtime = "dlsym";
    }
    return (void *)(intptr_t)fp;
}

char *dlerror(void) {
    static char errstr[88];

    if (var.err_code) {
        sprintf(errstr, "%s error #%ld", var.err_runtime, var.err_code);
        return errstr;
    } else {
        return NULL;
    }
}
