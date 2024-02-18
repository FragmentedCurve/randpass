/*
  Uses various predefined OS and compiler directives to determine
  where alloca is defined.

  Builtin compiler implementations of alloca are preferred over
  platform implementations.
 */

#ifndef _FINDALLOCA_H_
#define _FINDALLOCA_H_

#if defined(__GNUC__)
    /* https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-_005f_005fbuiltin_005falloca */
    #define alloca __builtin_alloca
#elif defined(__clang__)
    /* https://clang.llvm.org/docs/LanguageExtensions.html#builtin-alloca */
    #define alloca __builtin_alloca
#elif defined(__linux__)
    /* https://man7.org/linux/man-pages/man3/alloca.3.html */
    #include <alloca.h>
#elif defined(__FreeBSD__)
    /* https://man.freebsd.org/cgi/man.cgi?alloca */
    #include <stdlib.h>
#elif defined(_WIN32)
    /* https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-170 */
    #include <malloc.h>
    #define alloca _malloca
#else
    #error "Could not find 'alloca'."
#endif

#endif /* _FINDALLOCA_H_ */
