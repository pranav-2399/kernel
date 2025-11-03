#include "Unity.h"

extern void test_exceptions(void);

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UnityBegin("kernel-tests");
    RUN_TEST(test_exceptions);
    return UnityEnd();
}