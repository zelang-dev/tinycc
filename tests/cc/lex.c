// Copyright 2012 Rui Ueyama. Released under the MIT license.

#include "test.h"

#define stringify(x) %:x
#define paste(x, y) x%:%:y

static void escape() {
    int value = 10;
    expect(10, val\
ue);
    expect_string("a   bc", "a\   bc");
}

static void newline() {


#
}

static void dollar() {
    int $ = 1;
    expect(1, $);
    int $2 = 2;
    expect(2, $2);
    int a$ = 3;
    expect(3, a$);
}

void testmain() {
    print("lexer");
    escape();
    newline();
    dollar();
}
