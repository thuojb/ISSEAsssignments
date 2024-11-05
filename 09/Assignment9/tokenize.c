/*
 * tokenize.c
 * 
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: <your name here>
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenize.h"
#include "token.h"
#include "clist.h"

// Forward-declare the CList type, the associated _clist struct, and the _cl_node struct
struct _cl_node {
  Token element;
  struct _cl_node *next;
};

struct _clist {
  struct _cl_node *head;
  int length;
};

// Documented in .h file
const char *TT_to_str(TokenType tt)
{
  switch(tt) {
  case TOK_VALUE:
    return "VALUE";
  case TOK_PLUS:
    return "PLUS";
  case TOK_MINUS:
    return "MINUS";
  case TOK_MULTIPLY:
    return "MULTIPLY";
  case TOK_DIVIDE:
    return "DIVIDE";
  case TOK_POWER:
    return "POWER";
  case TOK_OPEN_PAREN:
    return "OPEN_PAREN";
  case TOK_CLOSE_PAREN:
    return "CLOSE_PAREN";
  case TOK_END:
    return "(end)";
  case TOK_INVALID:
    return "INVALID";
  default:
    return "UNKNOWN";
  }
}


// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  CList tokens = CL_new();


  //
  // TODO: Add your code here
  //

  return tokens;
}



// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (tokens->length == 0) {
    return TOK_END;
  }

  return tokens->head->element.type;
}


// Documented in .h file
Token TOK_next(CList tokens)
{
  if (tokens->length == 0) {
    Token invalid_token = { .type = TOK_INVALID, .value = 0 };
    return invalid_token;
  }

  return tokens->head->element;
}


// Documented in .h file
void TOK_consume(CList tokens)
{
  if (tokens->length > 0) {
    CL_pop(tokens);
  }
}


static void TOK_print_helper(int pos, Token element, void* data) {
    printf("Token %d: %s, Value: %f\n", pos, TT_to_str(element.type), element.value);
}

// Documented in .h file
void TOK_print(CList tokens) {
    CL_foreach(tokens, TOK_print_helper, NULL);
}