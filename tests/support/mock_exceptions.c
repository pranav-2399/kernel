// Host mock of exception_handler used for unit tests. Prints to stdout instead of halting.

#include <stdio.h>

void exception_handler(unsigned int int_no, unsigned int err_code) {
    printf("MOCK exception handler called: int=%u err=%u\n", int_no, err_code);
}