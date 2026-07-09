#include <stdio.h>
#include <pthread.h>

#define	CHECK(var,fmt,val)	printf(fmt "\n",var); if (var != val) errors = 1

typedef enum { tls_a, tls_b, tls_c } tls_enum_type;
typedef struct { int tls_d, tls_e, tls_f; } tls_struct_type;

__thread char tls_char = 42;
__thread short tls_short = 43;
__thread int tls_init = 44;
__thread long long tls_long_long = 45;
__thread int tls_zero;
__thread float tls_float = 46.0;
__thread double tls_double = 47.0;
__thread long double tls_long_double = 48.0;
__thread int *tls_ptr = (int *)49;
__thread tls_enum_type tls_enum = tls_b;
__thread tls_struct_type tls_struct = { 50, 51, 52 };

static int check(void)
{
    int errors = 0;

    CHECK(tls_char, "%d", 42);
    CHECK(tls_short, "%d", 43);
    CHECK(tls_init, "%d", 44);
    CHECK(tls_long_long, "%lld", 45);
    CHECK(tls_zero, "%d", 0);
    CHECK(tls_float, "%g", 46.0);
    CHECK(tls_double, "%g", 47.0);
    CHECK(tls_long_double, "%Lg", 48.0);
    CHECK(tls_ptr, "%p", (int *)49);
    CHECK(tls_enum, "%d", tls_b);
    CHECK(tls_struct.tls_d, "%d", 50);
    CHECK(tls_struct.tls_e, "%d", 51);
    CHECK(tls_struct.tls_f, "%d", 52);
    return errors;
}

static void *thread_func(void *arg)
{
    (void)arg;
    long errors = check();

    tls_char = 10;
    tls_short = 20;
    tls_init = 30;
    tls_long_long = 40;
    tls_zero = 50;
    tls_float = 60.0;
    tls_double = 70.0;
    tls_long_double = 80.0;
    tls_ptr = (int *)90;
    tls_enum = tls_c;
    tls_struct.tls_d = 100;
    tls_struct.tls_e = 110;
    tls_struct.tls_f = 120;

    CHECK(tls_char, "%d", 10);
    CHECK(tls_short, "%d", 20);
    CHECK(tls_init, "%d", 30);
    CHECK(tls_long_long, "%lld", 40);
    CHECK(tls_zero, "%d", 50);
    CHECK(tls_float, "%g", 60.0);
    CHECK(tls_double, "%g", 70.0);
    CHECK(tls_long_double, "%Lg", 80.0);
    CHECK(tls_enum, "%d", tls_c);
    CHECK(tls_ptr, "%p", (int *)90);
    CHECK(tls_struct.tls_d, "%d", 100);
    CHECK(tls_struct.tls_e, "%d", 110);
    CHECK(tls_struct.tls_f, "%d", 120);

    return (void *)errors;
}

int main()
{
    pthread_t t;
    void *ret;
    int errors = check();

    pthread_create(&t, NULL, thread_func, NULL);
    pthread_join(t, &ret);

    return (ret ? 1 : 0) | errors | check();
}
