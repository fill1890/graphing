/* DrewDotCo 08/05/2022
 *
 * Very basic graph library for handling DAGs
 *
 * Goals: Read in relations from file, store, give topological sort
 *
 * Written bc origin's doing it and it sounded interesting
 */

#include <malloc.h>

#include "graph.h"
#include "hash.h"
#include "list.h"
#include "dbg.h"

// Make a new graph
Graph *new_graph(void)
{
    Graph *new = malloc(sizeof(Graph));
    if(!new) return NULL;

    new->length = 0;
    new->start = NULL;
    new->end = NULL;
    return new;
}

Value *new_value(char item[])
{
    Value *new = malloc(sizeof(Value) + strlen(item) + 1); // yikes (add 1 for null byte)
    if(!new) return NULL;

    new->higher = new_list();
    new->lower = new_list();

    new->id = hash(item);
    strcpy(new->value, item);

    new->prev = NULL;
    new->next = NULL;
    new->to_transfer = 0;

    return new;
}

// Predec because _tree_rec and _l_rec are codependent
static int g_resolve_tree_rec(Value *leaf, Value *root);

// Wrapper function for _tree_rec to handle the higher/lower lists
static int g_resolve_l_rec(Item *item, Value *root)
{
    // run tree_rec for current item
    int err = g_resolve_tree_rec((Value *)item->value, root);
    if(err) return err;

    // continue to next item
    if(item->next) return g_resolve_l_rec(item->next, root);

    return 0;
}

static int g_resolve_tree_rec(Value *leaf, Value *root)
{
    // stop and return an error if we find the root value
    // specifically this will occur if any value higher than the relation currently applying
    //   depends on being lower than the greater one
    // essentially making sure we're not accidentally making a cyclic graph
    if(leaf->id == root->id) return ERR_RELATIONAL_CONFLICT;

    // set the transfer flag so we know to transfer it later
    leaf->to_transfer = 1;

    // resolve the higher tree if it exists
    if(leaf->higher->length != 0) return g_resolve_l_rec(leaf->higher->start, root);

    return 0;
}

static int g_resolve_tree(Value *trunk, Value *root)
{
    // Given a root, traverse its higher values and ensure there are no conflicts, while also setting transfer flags on each value that needs to be transferred
    // just a wrapper around the recursive function
    // left here because it's clearer and i might need to change it later
    return g_resolve_tree_rec(trunk, root);
}

// Shift an item (shift) before another value (pivot)
// Called from a recursive function, doesn't do any fancy handling itself
static void g_shift_before(Graph *graph, Value *pivot, Value *shift)
{
    // Break current links
    Value *prev = shift->prev;
    Value *next = shift->next;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
    shift->next = NULL;
    shift->prev = NULL;

    // Check for updates to the graph
    if(graph->start == shift) graph->start = next;
    if(graph->end == shift) graph->end = prev;
    graph->length -= 1;

    // Reinsert, we're just reusing the insertion function cos it's what we need
    g_insert_before(graph, pivot, shift);
}

// Recursive transfer function
//
// Shifts any values with the transfer flag to above the pivot
// Preserves orginal order, just moves them up relative to the pivot
static void g_transfer_rec(Graph *graph, Value *value, Value *pivot, int post_pivot)
{

    // Get the next value so we don't lose it after shift
    Value *next = value->next;

    // Middle case: hit pivot, skip value while setting post_pivot true
    if(value == pivot) {
        g_transfer_rec(graph, next, pivot, 1);
        return;
    }

    // Late case: After pivot and needs transfer
    // Specifically to exclude locations already before the pivot, which won't need to be moved
    if(post_pivot && value->to_transfer) {
        g_shift_before(graph, pivot, value);
    }

    // Early and all late cases: Reset to_transfer, continue
    value->to_transfer = 0;
    if(next) g_transfer_rec(graph, next, pivot, post_pivot);
}

// Wrapper around the recursive transfer function
static void g_transfer(Graph *graph, Value *pivot)
{
    // start with initial flag for post_pivot set to false
    g_transfer_rec(graph, graph->start, pivot, 0);
}

// Apply a new relation
// Will create new items if not present
int g_apply_relation(Graph *graph, char greater[], char lesser[])
{
    // Case 1: Both items present, no swap needed
    // Case 2: Both items present, swap needed but no conflicting relation
    // Case 3: Both items present, swap needed but conflicting relation
    // Case 4: One or both items not yet present

    // Prep for finding items
    int greater_i = -1;
    int lesser_i = -1;
    unsigned long greater_id = hash(greater);
    unsigned long lesser_id = hash(lesser);

    // Find items, if they exist
    Value *greater_v = g_find(graph, greater_id, &greater_i);
    Value *lesser_v = g_find(graph, lesser_id, &lesser_i);

    if(greater_v && lesser_v && greater_i < lesser_i) {
        // Case 1: Both present and no swap needed
        // TODO: Make sure we don't insert duplicates
        // Just add relations to the lists of higher and lower values
        l_push(greater_v->lower, lesser_v);
        l_push(lesser_v->higher, greater_v);
    } else if(greater_v && lesser_v && greater_i > lesser_i) {
        // Case 2 and 3

        // Resolve the tree to find items that need to be transferred
        int err = g_resolve_tree(greater_v, lesser_v);

        // check for case 3
        if(err == ERR_RELATIONAL_CONFLICT) {
            log_err("Conflict found! Cannot resolve %s > %s", greater, lesser);
            return err;
        }

        // resolve the graph transfers
        g_transfer(graph, lesser_v);

        // add relattions
        l_push(greater_v->lower, lesser_v);
        l_push(lesser_v->higher, greater_v);
    } else {
        // Case 4: need item

        // Make sure they both exist and create if not
        if(!greater_v) greater_v = new_value(greater);
        if(!lesser_v) lesser_v = new_value(lesser);


        // Add relations
        l_push(greater_v->lower, lesser_v);
        l_push(lesser_v->higher, greater_v);

        if(greater_i == -1 && lesser_i == -1) {
            // Neither exist, add them in order
            g_push(graph, greater_v);
            g_push(graph, lesser_v);
        } else if(greater_i == -1) {
            // Insert greater before lesser
            g_insert_before(graph, lesser_v, greater_v);
        } else if(lesser_i == -1) {
            // Insert lesser after greater
            g_insert_after(graph, greater_v, lesser_v);
        } else {
            // Error case - why do they both have an index if one of them wasn't defined?
            // TODO: update return values for consistency
            return -1;
        }
    }

    return 0;
}

// recursive function to get sorted list
// this uses a head:tail format common to Haskell and other functional languages
// so we assign the head (list[0]) then recurse over the rest (list[1:])
// slightly hacky in c but it also works nicely so
static char **g_sorted_rec(Value *value, char **list)
{
    // end of list case
    if(!value) return list;

    // assign the start of the current list
    list[0] = value->value;
    // continue with the tail of the list
    g_sorted_rec(value->next, &list[1]);
    return list;
}

// get the sorted graph as an array of strings
// no extra runtime logic as it's already sorted, just get the values0
char **g_sorted(Graph *graph, int *size)
{
    // special case with no values
    if(graph->length == 0) return NULL;

    // dynamically allocate an array of sufficient size
    char **list = malloc(sizeof(char *) * (unsigned long)graph->length);

    // set the size appropriately and start recursion
    *size = graph->length;
    return g_sorted_rec(graph->start, list);
}

// find a value in the graph (internal recursion)
static Value *g_find_rec(Value *value, unsigned long search, int *index)
{
    // Case 1: End of list and value not found, return null
    // Index will be reset by g_find
    if(!value) return NULL;

    if(index) *index += 1;

    // Case 2: IDs equal and found, return value
    // index should be correct from recursion
    if(value->id == search) {
        return value;
    }

    // Case 3: Not yet found, continue searching
    return g_find_rec(value->next, search, index);
}

// find a value
Value *g_find(Graph *graph, unsigned long search, int *index)
{
    // ignore the index if we haven't got one
    if(index) *index = 0;
    Value *found = g_find_rec(graph->start, search, index);

    // return if found
    if(found) return found;

    // else not found
    if(index) *index = -1;
    return NULL;
}

// push an item onto the end
// same implementation as l_push
void g_push(Graph *graph, Value *value)
{
    Value *before = graph->end;

    if(!graph->start) graph->start = value;
    if(before) before->next = value;
    value->prev = before;
    graph->end = value;
    graph->length += 1;
}

int g_insert_before(Graph *graph, Value *after, Value *new)
{
    // Insert 1 [] 3 <-2, given 3

    // Get 1
    Value *prev = after->prev;

    // Graph operations
    if(graph->start == after) graph->start = new;
    graph->length += 1;

    // 1.next = 2
    if(prev) prev->next = new;
    // 3.prev = 2
    after->prev = new;
    // 2.next = 3
    new->next = after;
    // 2.prev = 1
    new->prev = prev;

    return 0;
}

int g_insert_after(Graph *graph, Value *before, Value *new)
{
    // Insert 1 [] 3 <- 2, given 1

    // Get 3
    Value *next = before->next;

    // Graph operations
    // Set end if at end
    if(graph->end == before) graph->end = new;
    // Increase length
    graph->length += 1;

    // 1.next = 2
    before->next = new;
    // 3.prev = 2
    if(next) next->prev = new;
    // 2.next = 3
    new->next = next;
    // 2.prev = 1
    new->prev = before;

    return 0;
}

// recursive print for the higher and lower lists
static void g_print_l_rec(Item *item)
{
    // end cases
    if(!item) return;
    if(!item->value) return;

    printf("%li, ", (unsigned long int)((Value *)item->value)->id);

    g_print_l_rec(item->next);
}

// recursive wrapper for printing the lists
static void g_print_l(List *list)
{
    printf("[");
    g_print_l_rec(list->start);
    printf("]\n");
}

// recursive print for the graph
static void g_print_rec(Value *value)
{
    if(!value) return;

    if(value->to_transfer) {
        // add a * if it's been marked for transfer
        printf("[%li]: %s (*)\n", value->id, value->value);
    } else {
        printf("[%li]: %s\n", value->id, value->value);
    }
    // higher and lower lists
    printf("  higher: "); g_print_l(value->higher);
    printf("  lower: "); g_print_l(value->lower);

    g_print_rec(value->next);
}

void g_print(Graph *graph)
{
    printf("Length %i\n", graph->length);
    g_print_rec(graph->start);
}

// Recursively free items in a list
static void g_free_rec(Value *value)
{
    if(value->next) g_free_rec(value->next);

    if(value->higher) l_free(value->higher);
    if(value->lower) l_free(value->lower);

    free(value);
}

// Free a graph
// Will also destroy any items
void g_free(Graph *graph)
{
    if(graph->start) g_free_rec(graph->start);

    free(graph);
}
