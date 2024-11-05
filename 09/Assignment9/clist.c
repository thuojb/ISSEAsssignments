/*
 * clist.c
 * 
 * Linked list implementation for ISSE Assignment 9
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"

Token invalid_token = { .type = TOK_INVALID, .value = 0 };

#define DEBUG

struct _cl_node {
  Token element;
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
_CL_new_node(Token element, struct _cl_node *next)
{
  struct _cl_node* new = (struct _cl_node*) malloc(sizeof(struct _cl_node));
  assert(new);

  new->element = element;
  new->next = next;

  return new;
}

/*
 * Documented in .h file
 */
CList CL_new()
{
  CList list = (CList) malloc(sizeof(struct _clist));
  assert(list);

  list->head = NULL;
  list->length = 0;

  return list;
}

/*
 * Documented in .h file
 */
void CL_free(CList list)
{
  struct _cl_node *current = list->head;
  struct _cl_node *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  free(list);
}

/*
 * Documented in .h file
 */
int CL_length(CList list)
{
  return list->length;
}

/*
 * Documented in .h file
 */
void CL_append(CList list, Token element)
{
  struct _cl_node *new = _CL_new_node(element, NULL);

  if (list->head == NULL) {
    list->head = new;
  } else {
    struct _cl_node *last = list->head;
    while (last->next != NULL) {
      last = last->next;
    }
    last->next = new;
  }

  list->length++;
}

/*
 * Documented in .h file
 */
Token CL_nth(CList list, int index)
{
  struct _cl_node *current = list->head;
  int i;

  if (index < 0) {
    index += list->length;
  }

  if (index < 0 || index >= list->length) {
    Token invalid_token = { .type = TOK_INVALID, .value = 0 };
    return invalid_token;
  }

  for (i = 0; i < index && current != NULL; i++) {
    current = current->next;
  }

  return current->element;
}

/*
 * Documented in .h file
 */
Token CL_pop(CList list)
{
  if (list->head == NULL) {
    Token invalid_token = { .type = TOK_INVALID, .value = 0 };
    return invalid_token;
  }

  struct _cl_node *first = list->head;
  Token element = first->element;
  list->head = first->next;
  list->length--;
  free(first);

  return element;
}

/*
 * Documented in .h file
 */
void CL_foreach(CList list, CListForEachFn fn, void *data)
{
  struct _cl_node *current = list->head;
  int pos = 0;

  while (current != NULL) {
    fn(pos, current->element, data);
    current = current->next;
    pos++;
  }
}

/*
 * Documented in .h file
 */
void CL_print(CList list)
{
  struct _cl_node *current = list->head;
  int pos = 0;

  while (current != NULL) {
    printf("Token: %s, Value: %f\n", TT_to_str(current->element.type), current->element.value);
    current = current->next;
    pos++;
  }
}