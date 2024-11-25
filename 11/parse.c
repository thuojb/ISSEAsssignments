/*
 * parse.c
 * 
 * Recursive descent parser for ISSE Assignment 9
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "tokenize.h"
#include "token.h"
#include "expr_tree.h"

/*
 * Forward declarations for the functions (rules) to produce the
 * ExpressionWhizz grammar.
 */
static ExprTree assignment(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree additive(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree multiplicative(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree exponential(CList tokens, char *errmsg, size_t errmsg_sz);
static ExprTree primary(CList tokens, char *errmsg, size_t errmsg_sz);

// New function for handling assignment, following right-associativity
static ExprTree assignment(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree left = additive(tokens, errmsg, errmsg_sz);
  if (!left)
    return NULL;

  if (TOK_next_type(tokens) == TOK_EQUAL) {
    TOK_consume(tokens);
    
    ExprTree right = assignment(tokens, errmsg, errmsg_sz);
    if (!right) {
      ET_free(left);
      return NULL;
    }
    
    left = ET_node(OP_ASSIGN, left, right);
  }
  
  return left;
}

static ExprTree additive(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree left = multiplicative(tokens, errmsg, errmsg_sz);
  if (!left) 
    return NULL;
    
  while (TOK_next_type(tokens) == TOK_PLUS || TOK_next_type(tokens) == TOK_MINUS) {
    TokenType op = TOK_next_type(tokens);
    TOK_consume(tokens);
    
    ExprTree right = multiplicative(tokens, errmsg, errmsg_sz);
    if (!right) {
      ET_free(left);
      return NULL;
    }
    
    left = ET_node(op == TOK_PLUS ? OP_ADD : OP_SUB, left, right);
  }
  
  return left;
}

static ExprTree multiplicative(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree left = exponential(tokens, errmsg, errmsg_sz);
  if (!left) 
    return NULL;
    
  while (TOK_next_type(tokens) == TOK_MULTIPLY || TOK_next_type(tokens) == TOK_DIVIDE) {
    TokenType op = TOK_next_type(tokens);
    TOK_consume(tokens);
    
    ExprTree right = exponential(tokens, errmsg, errmsg_sz);
    if (!right) {
      ET_free(left);
      return NULL;
    }
    
    left = ET_node(op == TOK_MULTIPLY ? OP_MUL : OP_DIV, left, right);
  }
  
  return left;
}

static ExprTree exponential(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree left = primary(tokens, errmsg, errmsg_sz);
  if (!left)
    return NULL;
    
  if (TOK_next_type(tokens) == TOK_POWER) {
    TOK_consume(tokens);
    
    ExprTree right = exponential(tokens, errmsg, errmsg_sz);  // Note: recursive call to self for right-associativity
    if (!right) {
      ET_free(left);
      return NULL;
    }
    
    left = ET_node(OP_POW, left, right);
  }
  
  return left;
}

static ExprTree primary(CList tokens, char *errmsg, size_t errmsg_sz)
{
  ExprTree ret = NULL;

  if (TOK_next_type(tokens) == TOK_VALUE) {
    ret = ET_value(TOK_next(tokens).value);
    TOK_consume(tokens);

  } else if (TOK_next_type(tokens) == TOK_OPEN_PAREN) {
    TOK_consume(tokens);
    
    ret = additive(tokens, errmsg, errmsg_sz);
    if (!ret)
      return NULL;
      
    if (TOK_next_type(tokens) != TOK_CLOSE_PAREN) {
      snprintf(errmsg, errmsg_sz, "Expected closing parenthesis");
      ET_free(ret);
      return NULL;
    }
    TOK_consume(tokens);

  } else if (TOK_next_type(tokens) == TOK_MINUS) {
    TOK_consume(tokens);

    ret = primary(tokens, errmsg, errmsg_sz);
    if (ret == NULL) 
      return NULL;
    ret = ET_node(UNARY_NEGATE, ret, NULL);

  } else {
    snprintf(errmsg, errmsg_sz, "Unexpected token %s", TT_to_str(TOK_next_type(tokens)));
    return NULL;
  }

  return ret;
}

// Main parsing function with support for assignment at top level
ExprTree Parse(CList tokens, char *errmsg, size_t errmsg_sz)
{
  if (CL_length(tokens) == 0) {
    snprintf(errmsg, errmsg_sz, "Empty expression");
    return NULL;
  }
  
  ExprTree result = assignment(tokens, errmsg, errmsg_sz);
  if (!result)
    return NULL;
    
  if (TOK_next_type(tokens) != TOK_END) {
    snprintf(errmsg, errmsg_sz, "Unexpected token %s", TT_to_str(TOK_next_type(tokens)));
    ET_free(result);
    return NULL;
  }
  
  return result;
}