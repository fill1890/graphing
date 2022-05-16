// Test linked list implementation

#include "minunit.h"
#include "../src/list.h"
#include "../src/dbg.h"

mu_suite_start();

static List *t_list = NULL;

static int int1 = 1;
static int int2 = 2;
static int int3 = 3;

static char *test_new(void)
{
    t_list = new_list();

    mu_assert(t_list, "List not created")
    return NULL;
}

static char *test_push(void)
{
    l_push(t_list, &int1);

    mu_assert(t_list->start != NULL, "Start of list not initialised correctly")
    mu_assert(t_list->end != NULL, "End of list not initialised correctly")

    mu_assert(*(int *)t_list->start->value == 1, "1 not at start of list")
    mu_assert(*(int *)t_list->end->value == 1, "1 not at end of list")
    mu_assert(t_list->length == 1, "List length incorrect after 1")

    l_push(t_list, &int2);
    mu_assert(*(int *)t_list->start->value == 1, "1 not at start of list")
    mu_assert(*(int *)t_list->end->value == 2, "2 not at end of list, got %i", *(int *)t_list->end->value)
    mu_assert(t_list->length == 2, "List length incorrect after 3")

    l_push(t_list, &int3);
    mu_assert(*(int *)t_list->start->value == 1, "1 not at start of list")
    mu_assert(*(int *)t_list->end->value == 3, "3 not at end of list, got %i", *(int *)t_list->end->value)
    mu_assert(t_list->length == 3, "List length incorrect after 3")

    return NULL;
}

static char *test_index(void)
{
    void *value = NULL;

    value = l_index(t_list, 2);
    mu_assert(value == &int3, "3 not at index 2")

    value = l_index(t_list, 1);
    mu_assert(value == &int2, "2 not at index 1")

    value = l_index(t_list, 0);
    mu_assert(value == &int1, "1 not at index 0")

    return NULL;
}

static char *test_pop(void)
{
    void *value = NULL;

    value = l_pop(t_list);
    mu_assert(value == &int3, "3 not popped correctly")
    mu_assert(*(int *)t_list->start->value == 1, "1 not at start of list")
    mu_assert(*(int *)t_list->end->value == 2, "2 not at end of list")
    mu_assert(t_list->length == 2, "List length incorrect after 3")

    value = l_pop(t_list);
    mu_assert(value == &int2, "2 not popped correctly")
    mu_assert(*(int *)t_list->start->value == 1, "1 not at start of list")
    mu_assert(*(int *)t_list->end->value == 1, "1 not at end of list")
    mu_assert(t_list->length == 1, "List length incorrect after 2")

    value = l_pop(t_list);
    mu_assert(value == &int1, "1 not popped correctly")
    mu_assert(t_list->start == NULL, "List not empty (start)")
    mu_assert(t_list->end == NULL, "List not empty (end)")
    mu_assert(t_list->length == 0, "List length incorrect after 1")

    return NULL;
}

static char *all_tests(void)
{
    mu_run_test(test_new)
    mu_run_test(test_push)
    mu_run_test(test_index)
    mu_run_test(test_pop)

    l_free(t_list);

    return NULL;
}

RUN_TESTS(all_tests)
