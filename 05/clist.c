/*
 * clist.c
 * 
 * Linked list implementation for ISSE Assignment 5
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "clist.h"

#define DEBUG

struct _cl_node {
  CListElementType element;
  struct _cl_node *next;
};

struct _clist {
  struct _cl_node *head;
  int length;
};



/*
 * Create (malloc) a new _cl_node and populate it with the supplied
 * values
 *
 * Parameters:
 *   element, next  the values for the node to be created
 * 
 * Returns: The newly-malloc'd node, or NULL in case of error
 */
static struct _cl_node*
_CL_new_node(CListElementType element, struct _cl_node *next)
{
  struct _cl_node* new = (struct _cl_node*) malloc(sizeof(struct _cl_node));

  assert(new);

  new->element = element;
  new->next = next;

  return new;
}



// Documented in .h file
CList CL_new()
{
  CList list = (CList) malloc(sizeof(struct _clist));
  assert(list);

  list->head = NULL;
  list->length = 0;

  return list;
}



// Documented in .h file
void CL_free(CList list)
{
  if (!list) return;

  struct _cl_node* current = list->head;
  while (current!= NULL) {
    struct _cl_node *temp = current;
    current = current->next;
    free(temp);
  }
  list->head = NULL;
  free(list);
}



// Documented in .h file
int CL_length(CList list)
{
  assert(list);
#ifdef DEBUG
  // In production code, we simply return the stored value for
  // length. However, as a defensive programming method to prevent
  // bugs in our code, in DEBUG mode we walk the list and ensure the
  // number of elements on the list is equal to the stored length.

  int len = 0;
  for (struct _cl_node *node = list->head; node != NULL; node = node->next)
    len++;

  printf("Calculated length: %d, Stored length: %d\n", len, list->length);

  assert(len == list->length);
#endif // DEBUG

  return list->length;
}



// Documented in .h file
void CL_print(CList list)
{
  assert(list);

  int num = 0;
  for (struct _cl_node *node = list->head; node != NULL; node = node->next)
    printf("  [%d]: %s\n", num++, node->element);
}



// Documented in .h file
void CL_push(CList list, CListElementType element)
{
  assert(list);
  list->head = _CL_new_node(element, list->head);
  list->length++;
}



// Documented in .h file
CListElementType CL_pop(CList list)
{
  assert(list);

  struct _cl_node *popped_node = list->head;

  if (popped_node == NULL)
    return INVALID_RETURN;

  CListElementType ret = popped_node->element;

  // unlink previous head node, then free it
  list->head = popped_node->next;
  free(popped_node);
  // we cannot refer to popped node any longer

  list->length--;

  return ret;
}



// Documented in .h file
void CL_append(CList list, CListElementType element)
{
  assert(list);
  struct _cl_node *new_node = _CL_new_node(element, NULL);
  if (!list->head) {
    list->head = new_node;
  } else {
    struct _cl_node *current = list->head;
    while (current->next) {
      current = current->next;
    }
    current->next = new_node;
  }
  list->length++;
}




// Documented in .h file
CListElementType CL_nth(CList list, int pos)
{
  assert(list);
  
  int len = CL_length(list);
  if (pos < -len || pos >= len) return INVALID_RETURN;  // checking for valid position
  
  struct _cl_node *current = list->head;  // start at the head of the list
  
  if (pos < 0) {
    pos += len;  // negative index
  }
  
  for (int i = 0; i < pos; i++) {
    if (current == NULL) {
	    return INVALID_RETURN; //current shoulf not be NULL
    }
    current = current->next;  // traversing to the specified position
  }
  return current ? current->element: INVALID_RETURN;  // element at the specified position
}



// Documented in .h file
bool CL_insert(CList list, CListElementType element, int pos)
{
  assert(list);
  int len = CL_length(list);
  if (pos < -len || pos > len) return false;  // checking for valid position

  if (pos < 0) pos += len + 1; // negative index adjusting

  struct _cl_node *new_node = _CL_new_node(element, NULL);
  if (pos == 0) {
    new_node->next = list->head;
    list->head = new_node;
  } else {
    struct _cl_node *current = list->head;
    for (int i = 0; i < pos - 1; i++) {
      current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
  }
  list->length++;
  return true;
}


    
// Documented in .h file
CListElementType CL_remove(CList list, int pos)
{
  assert(list);
  int len = CL_length(list);
  if (pos < -len || pos >= len) return INVALID_RETURN;

  struct _cl_node *to_remove;
  if (pos < 0) pos += len; // negative index adjustment

  if (pos == 0) {
    to_remove = list->head;
    list->head = to_remove->next;
  } else {
    struct _cl_node *current = list->head;
    for (int i = 0; i < pos - 1; i++) {
      current = current->next;
    }
    to_remove = current->next;
    current->next = to_remove->next;
  }
  CListElementType element = to_remove->element;
  free(to_remove);
  list->length--;
  return element;
}



// Documented in .h file
CList CL_copy(CList src_list)
{
  assert(src_list);

  CList new_list = CL_new();
  for(struct _cl_node *node = src_list->head; node; node = node->next) {
    CL_append(new_list, node->element);
  }
  return new_list;
}



// Documented in .h file
int CL_insert_sorted(CList list, CListElementType element)
{
  assert(list);
  struct _cl_node *new_node = _CL_new_node(element, NULL);

  if (!new_node){
	  return -1; //memory allocation failure
  }

  if (!list->head || strcmp(list->head->element, element) > 0) {
    new_node->next = list->head;
    list->head = new_node;
    list->length++;
    return 0;
  }
//    if (!list->head) {
//	    new_node->next = new_node; //point to iteself
//	    list->head = new_node;
//	    list->length++;
//	    return 0;
//    }
//    if (strcmp(list->head->element, element) > 0) {
//	    struct _cl_node *tail = list->head;
//	    while (tail->next != list->head) { // last node
//		    tail = tail->next;
//	    }
//	    tail->next = new_node; //last node to new node
//	    new_node->next = list->head;
//	    list->head = new_node;
//	    list->length++;
//
//	    return 0;
//    }

  struct _cl_node *current = list->head;
  int pos = 0;

  while (current->next && strcmp(current->next->element, element) < 0) {
    current = current->next;
    pos++;
  }
  new_node->next = current->next;
  current->next = new_node;
  list->length++;
  return pos + 1; // if element is already in the list, return its position
  //return -1;
}



// Documented in .h file
void CL_join(CList list1, CList list2)
{
  assert(list1);
  assert(list2);

  if (!list1->head) {
    list1->head = list2->head;
    } else {
      struct _cl_node *current = list1->head;
      while (current->next) {
        current = current->next;
      }
      current->next = list2->head;
    }
    
    list1->length += list2->length;
    list2->head = NULL;
    list2->length = 0;
}

// Documented in .h file
void CL_reverse(CList list)
{
  assert(list);
  struct _cl_node *prev = NULL;
  struct _cl_node *current = list->head;
  struct _cl_node *next;
  
  while(current) {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }
  list->head = prev;
}


// Documented in .h file
void CL_foreach(CList list, CL_foreach_callback callback, void *cb_data)
{
  assert(list);
  assert(callback);

  struct _cl_node *current = list->head;
  int pos = 0;

//  if (!current) return;
//
//  do {
//	  callback(current->element, cb_data, pos);
//
//	  current = current->next;
//	  pos++;
//  } while (current != list->head);
//}


  while(current) {
    callback(pos, current->element, cb_data);
    current = current->next;
    pos++;
  }
}




