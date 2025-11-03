// Unit test that calls the host-side mock of exception_handler.
// This avoids halting the host while verifying the call/link behavior.

#include "Unity.h"

/* Host mock provided in tests/support/mock_exceptions.c */
extern void exception_handler(unsigned int int_no, unsigned int err_code);

void test_exceptions(void) {
    /* Should return normally; mock prints a line to stdout. */
    exception_handler(3, 0);
    TEST_ASSERT_TRUE(1);
}