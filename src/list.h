/* DrewDotCo, 08/05/2022
 *
 * Basic linked list implementation
 */

#ifndef LIST_H
#define LIST_H

/* struct: Item
 *
 * Item in a linked list. Note that this is typically not exposed by list operation functions,
 *   and is only needed during direct list access
 *
 * Format:
 *   Item *next: Next item in list
 *   Item *previous: Previous item in list
 *   void *value: Pointer to value
 */
typedef struct item Item;

typedef struct item {
    Item *previous;
    Item *next;
    void *value;
} Item;


/* struct: List
 *
 * Double linked list
 *
 * Create with new_list and operate with l_* functions
 *
 * Format:
 *   Item *start: First item in list
 *   Item *end: Last item in list
 *   int length: Current length of list
 *
 * Avoid modifying attributes directly as it will break the list functions
 */
typedef struct list {
    Item *start;
    Item *end;
    int length;
} List;

/* function: List* new_list()
 *
 * Creates a new, empty linked list
 *
 * Returns a pointer to the list or NULL if error occured
 */
List* new_list(void);

/* function: int l_push(List *list, void *item)
 *
 * Push an item onto the end of a list
 *
 * list: list to push onto (must be initialised)
 * item: pointer to item to add
 *
 * Returns 0 on success, 1 if out of memory allocating item
 */
int l_push(List *list, void *item);

/* function: void *l_pop(List *list)
 *
 * Pop an item off the end of the list
 *
 * list: list to pop off
 *
 * Returns pointer to the item removed
 */
void *l_pop(List *list);

/* function: void *l_index(List *list, int n)
 *
 * Get the nth item in a list
 *
 * list: list to use
 * n: Index of item
 *
 * Returns item at index n, or NULL if out of bounds
 */
void *l_index(List *list, int n);

/* function: void l_free(List *list)
 *
 * Clear and free a list; use during cleanup
 *
 * Note that this a destructive operation
 */
void l_free(List *list);

#endif
