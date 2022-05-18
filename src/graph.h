/* DrewDotCo, 09/05/2022
 *
 * Basic DAG structure
 *
 * Values should be strings
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "list.h"

/* Errors
 *
 * ERR_RELATIONAL_CONFLICT: Error during relationship resolution; cyclic dependency
 */
enum g_error {
    ERR_RELATIONAL_CONFLICT = 1,
};

/* struct: Value
 *
 * Value in a graph. Avoid using directly
 *
 * higher: List of pointers to higher values (direct relations)
 * lower: List of pointers to lower values
 * prev: Previous value in graph
 * next: Next value in graph
 * id: Hashed value for id
 * to_transfer: Bool used during relationship resolution
 * value: String value
 */
typedef struct value Value;

typedef struct value {
    List *higher; // List[Value]
    List *lower;  // List[Value]
    Value *prev;
    Value *next;
    unsigned long id;
    int to_transfer;
    char value[];
} Value;

/* struct: Graph
 *
 * Representation of a DAG
 *
 * Create using new_graph and operate using g_* functions
 *
 * start: Start value
 * end: End value
 * length: Length of graph
 */
typedef struct graph {
    Value *start;
    Value *end;
    int length;
} Graph;

/* function: new_graph()
 *
 * Create a new empty graph
 *
 * Check if not null before using
 */
Graph *new_graph(void);

/* function: new_value(char item[])
 *
 * Creates a new value with value item
 * Returns new value, check if not null before using
 *
 * Does not add to graph!
 */
Value *new_value(char item[]);

/* function: apply_relation(Graph *graph, char greater[], char lesser[])
 *
 * Apply a new relation in the graph, where greater > lesser
 *
 * Note that the given arguments should be strings - if they don't exist in the
 *   graph, they will be automatically added
 *
 * This is the primary function for updating a graph
 *
 * Returns 0 on success or 1 on error
 */
int g_apply_relation(Graph *graph, char greater[], char lesser[]);

/* function: g_sorted(Graph *graph, int *size)
 *
 * Get the sorted graph as an array of strings
 * Returns a pointer to the start of the array
 * Sets size to the number of strings
 *
 * Returns NULL on an empty list
 */
char **g_sorted(Graph *graph, int *size);

/* function: g_find(Graph *graph, unsigned long id, int *i)
 *
 * Find a value by id in a graph
 *
 * Returns the value; i will be set to its index in the graph
 * Note there is not a function to lookup by index, use for comparisons
 *
 * Array should be freed with free() after use
 */
Value *g_find(Graph *graph, unsigned long id, int *i);

/* function: g_push(Graph *graph, Value *value)
 *
 * Push a value onto the end of a graph
 *
 * Typically only used internally
 */
void g_push(Graph *graph, Value *value);

/* function: g_insert_before(Graph *graph, Value *before, Value *value)
 *
 * Insert a value `value` into a graph before another value `before`
 *
 * Typically only used internally
 *
 * Returns non-zero on error (currently no error cases)
 */
int g_insert_before(Graph *graph, Value *before, Value *value);

/* function: g_insert_after(Graph *graph, Value *after, Value *value)
 *
 * Insert a value `value` into a graph after another value `after`
 *
 * Typically only used internally
 *
 * Returns non-zero on error (currently no error cases)
 */
int g_insert_after(Graph *graph, Value *after, Value *value);

/* function: g_print(Graph *graph)
 *
 * Print a graph, including length, values, and the higher and lower relations for each value
 *
 * Output subject to change
 */
void g_print(Graph *graph);

/* function: g_free(Graph *graph)
 *
 * Free a graph
 *
 * Should be used to destroy a graph when no longer in use
 */
void g_free(Graph *graph);

#endif
