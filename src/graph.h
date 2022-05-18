/* DrewDotCo, 09/05/2022
 *
 * Basic DAG structure
 *
 * Values should be strings
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "list.h"

enum g_error {
    ERR_RELATIONAL_CONFLICT = 1,
};

typedef struct value Value;

// Value in graph
typedef struct value {
    List *higher; // List[Value]
    List *lower;  // List[Value]
    Value *prev;
    Value *next;
    unsigned long id;
    int to_transfer;
    char value[];
} Value;

// Graph type
typedef struct graph {
    Value *start;
    Value *end;
    int length;
} Graph;

// Create a graph
Graph *new_graph(void);

// New value with item
Value *new_value(char item[]);

// Apply a new relation (will create items if not present) (err return)
int g_apply_relation(Graph *graph, char greater[], char lesser[]);

// Get the sorted list
char **g_sorted(Graph *graph, int *size);

// Find a value, returns value and the location (i)
Value *g_find(Graph *graph, unsigned long id, int *i);

int g_push(Graph *graph, Value *value);

int g_insert_before(Graph *graph, Value *before, Value *value);

int g_insert_after(Graph *graph, Value *after, Value *value);

void g_print(Graph *graph);

void g_free(Graph *graph);

#endif
