// Copyright 2012 Rui Ueyama. Released under the MIT license.

#include "test.h"
#include <stdbool.h>

static void test_primitives() {
    expect(1, sizeof(void));
    expect(1, sizeof(test_primitives));
    expect(1, sizeof(char));
    expect(1, sizeof(_Bool));
    expect(1, sizeof(bool));
    expect(2, sizeof(short));
    expect(4, sizeof(int));
    expect(8, sizeof(long long));
}

static void test_pointers(int s) {
    expect(s, sizeof(char *));
    expect(s, sizeof(short *));
    expect(s, sizeof(int *));
    expect(s, sizeof(long *));
}

static void test_unsigned() {
    expect(1, sizeof(unsigned char));
    expect(2, sizeof(unsigned short));
    expect(4, sizeof(unsigned int));
    expect(8, sizeof(unsigned long long));
}

static void test_literals() {
    expect(4, sizeof 1);
    expect(4, sizeof('a'));
    expect(4, sizeof(1.0f));
    expect(8, sizeof 1LL);
    expect(8, sizeof 1.0);
    expect(8, sizeof(1.0));
}

static void test_arrays() {
    expect(1, sizeof(char[1]));
    expect(7, sizeof(char[7]));
    expect(30, sizeof(char[3][10]));
    expect(32, sizeof(int[4][2]));
}

static void test_vars() {
    char a[] = { 1, 2, 3 };
    expect(3, sizeof(a));
    char b[] = "abc";
    expect(4, sizeof(b));
    expect(1, sizeof(b[0]));
    expect(1, sizeof((b[0])));
    expect(1, sizeof((b)[0]));
    char *c[5];
    char *(*d)[3];

#ifdef __arm__
    expect(20, sizeof(c));
    expect(4, sizeof(d));
    expect(12, sizeof(*d));
    expect(4, sizeof(**d));
#else
    expect(40, sizeof(c));
    expect(8, sizeof(d));
    expect(24, sizeof(*d));
    expect(8, sizeof(**d));
#endif
    expect(1, sizeof(***d));
    expect(4, sizeof((int)a));
}

static void test_struct() {
    expect(1, sizeof(struct { char a; }));
    expect(3, sizeof(struct { char a[3]; }));
    expect(5, sizeof(struct { char a[5]; }));
    expect(8, sizeof(struct { int a; char b; }));
    expect(12, sizeof(struct { char a; int b; char c; }));
    expect(24, sizeof(struct { char a; double b; char c; }));
    expect(24, sizeof(struct { struct { char a; double b; } x; char c; }));
}

static void test_constexpr() {
    char a[sizeof(char[4])];
    expect(4, sizeof(a));
}

void testmain() {
    print("sizeof");
    test_primitives();
#ifdef __arm__
    test_pointers(4);
#else
    test_pointers(8);
#endif
    test_unsigned();
    test_literals();
    test_arrays();
    test_vars();
    test_struct();
    test_constexpr();
}
