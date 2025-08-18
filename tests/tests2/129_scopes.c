int printf(const char*, ...);

#define myassert(x) \
    printf("%s:%d: %s : \"%s\"\n", __FILE__,__LINE__,(x)?"ok":"error",#x)

enum{ in = 0};

int main_1(){
    {
        myassert(!in);
        if(sizeof(enum{in=1})) myassert(in);
        myassert(!in); //OOPS
    }
    {
        myassert(!in);
        switch(sizeof(enum{in=1})) { default: myassert(in); }
        myassert(!in); //OOPS
    }
    {
        myassert(!in);
        while(sizeof(enum{in=1})) { myassert(in); break; }
        myassert(!in); //OOPS
    }
    {
        myassert(!in);
        do{ myassert(!in);}while(0*sizeof(enum{in=1}));
        myassert(!in); //OOPS
    }

    {
        myassert(!in);
        for(sizeof(enum{in=1});;){ myassert(in); break; }
        myassert(!in); //OK
    }
    {
        myassert(!in);
        for(;;sizeof(enum{in=1})){ myassert(in); break; }
        myassert(!in); //OK
    }
    {
        myassert(!in);
        for(;sizeof(enum{in=1});){ myassert(in); break; }
        myassert(!in); //OK
    }
    return 0;
}

/* --------------------------------------------- */
int main_2()
{
  char c = 'a';
  void func1(char c); /* param 'c' must not shadow local 'c' */
  func1(c);
  return 0;
}

void func1(char c)
{
    myassert(c == 'a');
}

struct st { int a; };

/* --------------------------------------------- */
int main_3()
{
  struct st func(void);
  struct st st = func(); /* not an 'incompatible redefinition' */
  myassert(st.a == 10);
  return 0;
}

struct st func(void)
{
  struct st st = { 10 };
  return st;
}

/* --------------------------------------------- */
int main()
{
    main_1();
    main_2();
    main_3();
    return 0;
}
