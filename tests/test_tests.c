// Check unit test framework is working

#include "minunit.h"

mu_suite_start();

static char *test_test(void)
{
    mu_assert(1, "1 is true")
    return NULL;
}

static char *all_tests(void)
{
    mu_run_test(test_test)

    return NULL;
}

RUN_TESTS(all_tests)
