// #include "libc.h"
#include <stdlib.h>

void __attribute__((optimize("no-stack-protector"))) __libc_init_main(int (*main)(int, char**,
                                                                                  char**)) {
    // TODO: Initialize libc
    int returnCode = main(1, (char*[]){"main"}, (void*)0);
    exit(returnCode);
    __builtin_unreachable();
}