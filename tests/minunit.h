/*
USAGE:
Include minunit.h
Include relevant header files
Call mu_suite_start()
Define individual test functions of type 'char *test()'
Terminate with 'return NULL'
Use mu_assert with assertion and error message to assert
Create all_tests function of type 'char *test()'
Terminate with 'return NULL'
Run each test with mu_run_test(test)
Terminate file with RUN_TESTS(all_tests)
*/

#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <dbg.h>
#include <stdlib.h>
#include <string.h>

#define mu_suite_start() static char *message = NULL

#define mu_assert(test, message, ...) if (!(test)) { log_err(message, ##__VA_ARGS__); return message; }

#define mu_run_test(test) debug("\n-----%s", " " #test); \
    message = test(); tests_run++; if(message) return message;

#define RUN_TESTS(name) int main(int argc, char* argv[]) {\
    argc++; \
    debug("----- RUNNING: %s\n", argv[0]);\
    char* result = name();\
    if (result != NULL) {\
        printf("\x1b[31mTEST FAILED\x1b[0m\n");\
    }\
    else {\
        printf("\x1b[32mALL TESTS PASSED\x1b[0m\n");\
    }\
    printf("Tests run: %d\n", tests_run);\
    exit(result != NULL);\
}

static int tests_run;

#endif
