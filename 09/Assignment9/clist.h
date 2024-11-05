/*
 * clist.h
 * 
 * Linked list implementation for ISSE Assignment 9
 *
 * Author: <your name here>
 */

#ifndef _CLIST_H_
#define _CLIST_H_

#include "token.h"

// struct _clist is defined in .c file
typedef struct _clist *CList;

/*
 * Create a new CList 
 *
 * Parameters: None
 * 
 * Returns: The new list
 */
CList CL_new();


/*
 * Destroy a list, calling free() on all malloc'd memory.
 *
 * Parameters:
 *   list   The list; if NULL, no action will occur
 * 
 * Returns: None
 */
void CL_free(CList list);



/*
 * Compute the length of a list
 *
 * Parameters:
 *   list   The list
 * 
 * Returns: The length of the list, or 0 if list is empty
 */
int CL_length(CList list);


/*
 * Append the specified element to the tail of the list
 *
 * Parameters:
 *   list     The list
 *   element  The element to append
 * 
 * Returns: None
 */
void CL_append(CList list, Token element);


/*
 * Return the Nth element, without modifying the list
 *
 * Parameters:
 *   list     The list
 *   index    Position to return
 * 
 * If index >= 0, the corresponding element will be returned, counting 0
 * as the head element.  So index == 0 will return the head element, and
 * index == 1 will return the second element on the list.
 *
 * If index < 0, the corresponding element counting from the end of
 * the list will be returned, so for instance index == -1 will return the
 * tail element and index == -2 will return the element before the tail
 * element.
 * 
 * index must be in the range [-length, length-1] inclusive. If index is
 * outside this range, returns INVALID_TOKEN.
 * 
 * Returns: The requested element, or INVALID_TOKEN if no element was found.
 */
Token CL_nth(CList list, int index);


/*
 * Remove the element from the head of the list and return 
 * it. If the list is empty, return INVALID_TOKEN.
 *
 * Parameters:
 *   list     The list
 * 
 * Returns: The popped item
 */
Token CL_pop(CList list);


typedef void (*CListForEachFn)(int pos, Token element, void *data);

/*
 * Iterate through the list; call the user-specified callback function
 * for each element.  Each call to callback will be of the form
 * 
 *   callback( <position>, <element>, <data> )
 *
 * Parameters:
 *   list       The list
 *   callback   The function to call
 *   data       Caller data to pass to the function
 * 
 * Returns: None
 */
void CL_foreach(CList list, CListForEachFn callback, void *data);

#endif /* _CLIST_H_ */