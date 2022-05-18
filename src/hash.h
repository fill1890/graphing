// Quick string hashing function
// Uses djb2, http://www.cse.yorku.ca/~oz/hash.html

#ifndef HASH_H
#define HASH_H

/* function: hash(char *str)
 *
 * Hash a string using djb2
 *
 * Returns hash
 */
unsigned long hash(char *str);

#endif
