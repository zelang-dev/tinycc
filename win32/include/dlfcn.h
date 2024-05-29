#ifndef __DLFCN_H__
#define __DLFCN_H__
/*
 * $Id: dlfcn.h,v 1.1 2003/04/29 20:21:35 tchamp Exp $
 * $Name:  $
 *
 * This file is set under public domain. Please read disclaimer.
 */
void *dlopen(const char *file, int mode);
int dlclose(void *handle);
void *dlsym(void *handle, const char *name);
char *dlerror(void);

/* These don't mean anything on windows */
#define RTLD_NEXT           ((void *) -1l)
#define RTLD_DEFAULT        ((void *) 0)
#define RTLD_LAZY           -1
#define RTLD_NOW            -1
#define RTLD_BINDING_MASK   -1
#define RTLD_NOLOAD         -1
#define RTLD_GLOBAL         -1
#define RTLD_LOCAL          (1 << 2)

#endif /* __DLFCN_H__ */
