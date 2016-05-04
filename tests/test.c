#include <stdio.h>
#include "minunit.h"

#include "test_jsonrpc.h"

void test_setup() {
    // Nothing
}

void test_teardown() {
    // Nothing
}

MU_TEST_SUITE(test_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    // test_jsonrpc.h
    MU_RUN_TEST(test_request_new);
    MU_RUN_TEST(test_jsonrpc_request_new);
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();

    return 0;
}
