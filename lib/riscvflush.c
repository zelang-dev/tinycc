/* riscvflush.c - flush the instruction cache

   __clear_cache is used in tccrun.c,  It is a built-in
   intrinsic with gcc.  However tcc in order to compile
   itself needs this function */

void __clear_cache(void *beg, void *end)
{
    __riscv64_clear_cache(beg, end);
}
