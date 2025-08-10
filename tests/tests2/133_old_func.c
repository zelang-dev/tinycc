int printf(const char *s, ...);

float fx(x)
float x;
{
    return 2.0 * x;
}

int
main(void)
{
    float fy();

    printf("%g %g\n", fx(2.0), fy(10.0));
    printf("%g %g\n", fx(2.0f), fy(10.0f));
}

float fy(x)
float x;
{
    return 3.0 * x;
}

