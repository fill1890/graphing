/* DrewDotCo 08/05/2022
 *
 * Very basic graph library for handling DAGs
 *
 * Goals: Read in relations from file, store, give topological sort
 *
 * Written bc origin's doing it and it sounded interesting
 */

/* Thoughts
 *
 * Plan to read from file that gives a series of relations a > b, c > d, a > c, etc each on a new line
 * Implement that as a separate function
 * Provide a sorted() function that returns an array (list? dl-list?) with the sorted order
 *
 * Data representation - depends on whether we need to retrieve the original structure later
 * - Could store the relations directly, and interpret & store at runtime - keeps original format but high runtime cost
 * - Alternatively store as a "tree"-style structure with links - complex form but more true to the system
 * - Or could store as a sorted array directly - less complicated and satisfies requirements but loses information
 *
 * I'm inclined to go with the second option because it's most "correct" in terms of the data
 *   being used, and the most interesting to implement
 *
 * What do we need?
 * - DAGs can be represented as a many-to-many relationship, where any one item can have multiple
 *   other items ranked higher or lower (or equal)
 * - Would be good to have a representation of a single item, with a list of higher and a list of lower items
 * - I also want a list of all the items in use so i can access them easily
 *
 * What if we used a hybrid approach? Sort them as they come in into a list, and store each relation
 * I might implement a linked list and sorted linked list bc why not
 * Provide a weighting function for a generic sorted linked list
 *
 * On further consideration: a generic sorted linked list is very complicated and I don't really want to deal with it
 * So I'll just do a non-generic one
 *
 * Structure: Sorted linked list of values, each of which contain a list of the values higher and lower than itself
 * Unfortunately assigning a weight to each item is difficult, so we can't really implement a binary search
 * Not that it works that well with a linked list anyway
 * So it won't be particularly well optimised
 *
 * DAGs are complex to work with because each item has no inherent value - it's only given one when
 *   described by its companions
 * A linked list works well for describing a sorted order, as items can be shifted around easily, but it's
 *   difficult to do optimised computations
 * Searches in particular are inefficient as we need to traverse the entire list in order to find something
 *
 * I'm planning to store values as strings since it works better
 * But this could theoretically be extended to generic objects, although they would need a unique id to avoid
 *   complicated deep matching
 * One issue is this results in a lot of string comparisons, so one good option would be to hash the strings into
 *   a uid for fast checking
 * But that's outside the scope of this project
 * Update: No longer outside the scope of this project, strings are now hashed and used as the id upon creation
 *
 * Edge cases and dealing with circular graphs are a pain
 *
 * Swapping Items and Avoiding Conflicts
 * So a possible conflict is when we say a > b > c, then we try to say c > a
 * This may not be a valid DAG (i have no idea) but I want to implement error checking for it anyway just in case
 * If we have, say, a > b and c > d, then we want to say d > a, we need to move both c and d above a, while also checking
 * for conflicts
 * I want to do a tree resolution that traverses the full set of all values higher than whatever we're swapping, then
 * sets a flag on each value, then iterates over the list and reapplies relations
 * Which should work
 * Some optimisation required
 * We'll also have to check each against a base to make sure we're not conflicting anywhere ._.
 * Imma just try it and see how I go
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

    new->length = 0;
    new->start = NULL;
    new->end = NULL;
    return new;
}

Value *new_value(char item[])
{
    Value *new = malloc(sizeof(Value) + strlen(item) + 1); // yikes
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

static int g_resolve_tree_rec(Value *leaf, Value *root);

// Wrapper to function to iterate through the list
static int g_resolve_l_rec(Item *item, Value *root)
{
    int err = g_resolve_tree_rec((Value *)item->value, root);
    if(err) return err;

    if(item->next) return g_resolve_l_rec(item->next, root);

    return 0;
}

static int g_resolve_tree_rec(Value *leaf, Value *root)
{
    if(leaf->id == root->id) return ERR_RELATIONAL_CONFLICT;

    leaf->to_transfer = 1;

    if(leaf->higher->length != 0) return g_resolve_l_rec(leaf->higher->start, root);

    return 0;
}

static int g_resolve_tree(Value *trunk, Value *root)
{
    // Given a root, traverse its higher values and ensure there are no conflicts, while also setting transfer flags on each value that needs to be transferred
    //
    // What is my control flow here even supposed to be 
    //
    // for value in root->higher
    //      check value != root
    //      set flag
    //      for value in value->higher, repeat
    return g_resolve_tree_rec(trunk, root);
}

static void g_shift_before(Graph *graph, Value *pivot, Value *shift)
{
    // Break
    Value *prev = shift->prev;
    Value *next = shift->next;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
    shift->next = NULL;
    shift->prev = NULL;

    if(graph->start == shift) graph->start = next;
    if(graph->end == shift) graph->end = prev;
    graph->length -= 1;

    // Reapply
    g_insert_before(graph, pivot, shift);
}

static void g_transfer_rec(Graph *graph, Value *value, Value *pivot, int post_pivot) {

    // Get the next value so we don't lose it after shift
    Value *next = value->next;

    // Middle case: hit pivot, skip while setting post_pivot true
    if(value == pivot) {
        g_transfer_rec(graph, next, pivot, 1);
        return;
    }

    // Late case: After pivot and needs transfer
    if(post_pivot && value->to_transfer) {
        g_shift_before(graph, pivot, value);
    }

    // Early and all late cases: Reset to_transfer, continue
    value->to_transfer = 0;
    if(next) g_transfer_rec(graph, next, pivot, post_pivot);

    return;
}

static void g_transfer(Graph *graph, Value *pivot)
{
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

    int greater_i = -1;
    int lesser_i = -1;

    unsigned long greater_id = hash(greater);
    unsigned long lesser_id = hash(lesser);


    Value *greater_v = g_find(graph, greater_id, &greater_i);
    Value *lesser_v = g_find(graph, lesser_id, &lesser_i);

    if(greater_v && lesser_v && greater_i < lesser_i) {
        // Case 0
        // TODO: Make sure we don't insert duplicates
        l_push(greater_v->lower, lesser_v);
        l_push(lesser_v->higher, greater_v);
    } else if(greater_v && lesser_v && greater_i > lesser_i) {
        // Case 2 and 3

        // Resolve the tree to find items that need to be transferred
        int err = g_resolve_tree(greater_v, lesser_v);

        if(err == ERR_RELATIONAL_CONFLICT) {
            log_err("Conflict found! Cannot resolve %s > %s", greater, lesser);
            return err;
        }

        g_transfer(graph, lesser_v);

        l_push(greater_v->lower, lesser_v);
        l_push(lesser_v->higher, greater_v);
    } else {
        // Case 4: need item

        // Make sure they both exist
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
            return -1;
        }
    }

    return 0;

}

static char **g_sorted_rec(Value *value, char **list)
{
    if(!value) return list;

    list[0] = value->value;
    g_sorted_rec(value->next, &list[1]);
    return list;
}

char **g_sorted(Graph *graph, int *size)
{
    if(graph->length == 0) return NULL;

    char **list = malloc(sizeof(char *) * (unsigned long)graph->length);

    *size = graph->length;
    return g_sorted_rec(graph->start, list);
}

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

    return g_find_rec(value->next, search, index);
}

Value *g_find(Graph *graph, unsigned long search, int *index)
{
    if(index) *index = 0;
    Value *found = g_find_rec(graph->start, search, index);

    if(found) return found;

    // else not found
    if(index) *index = -1;
    return NULL;
}

int g_push(Graph *graph, Value *value)
{
    Value *before = graph->end;

    if(!graph->start) graph->start = value;
    if(before) before->next = value;
    value->prev = before;
    graph->end = value;
    graph->length += 1;

    return 0;
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

static void g_print_l_rec(Item *item)
{
    if(!item) return;
    if(!item->value) return;

    printf("%li, ", (unsigned long int)((Value *)item->value)->id);

    g_print_l_rec(item->next);
}

static void g_print_l(List *list)
{
    printf("[");
    g_print_l_rec(list->start);
    printf("]\n");
}

static void g_print_rec(Value *value)
{
    if(!value) return;

    if(value->to_transfer) {
        printf("[%li]: %s (*)\n", value->id, value->value);
    } else {
        printf("[%li]: %s\n", value->id, value->value);
    }
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
