/* DrewDotCo 08/05/2022
 *
 * Very basic graph library for handling DAGs
 *
 * Goals: Read in relations from file, store, give topological sort
 *
 * Written bc origin's doing it and it sounded interesting
 */

/* Structure
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
 */
