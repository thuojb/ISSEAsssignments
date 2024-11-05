/*
 * tokenize.c
 * 
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"

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
  }

  __builtin_unreachable();
}

// Helper function to check if a character is a valid operator
static bool is_operator(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Helper function to check if a character is a valid parenthesis
static bool is_paren(char c) {
  return c == '(' || c == ')';
}

// Helper to get token type from operator char
static TokenType char_to_token_type(char c) {
  switch(c) {
    case '+': return TOK_PLUS;
    case '-': return TOK_MINUS;
    case '*': return TOK_MULTIPLY;
    case '/': return TOK_DIVIDE;
    case '^': return TOK_POWER;
    case '(': return TOK_OPEN_PAREN;
    case ')': return TOK_CLOSE_PAREN;
    default: return TOK_END;
  }
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  CList tokens = CL_new();
  const char *p = input;
  char *endptr;
  Token tok;
  
  // Skip leading whitespace
  while (isspace(*p)) p++;
  
  // Process each character
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    
    // Handle numbers
    if (isdigit(*p) || *p == '.') {
      // Use strtod to parse numbers including scientific notation
      tok.value = strtod(p, &endptr);
      
      // Check for invalid numbers
      if (p == endptr) {
        snprintf(errmsg, errmsg_sz, "Position %ld: Illegal numeric value", p - input + 1);
        CL_free(tokens);
        return NULL;
      }
      
      tok.type = TOK_VALUE;
      CL_append(tokens, tok);
      p = endptr;
      continue;
    }
    
    // Handle operators and parentheses
    if (is_operator(*p) || is_paren(*p)) {
      tok.type = char_to_token_type(*p);
      tok.value = 0;
      CL_append(tokens, tok);
      p++;
      continue;
    }
    
    // Invalid character
    snprintf(errmsg, errmsg_sz, "Position %ld: unexpected character %c", p - input + 1, *p);
    CL_free(tokens);
    return NULL;
  }

  return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (CL_length(tokens) == 0)
    return TOK_END;
  return CL_nth(tokens, 0).type;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
  if (CL_length(tokens) == 0) {
    Token end_token = {TOK_END, 0};
    return end_token;
  }
  return CL_nth(tokens, 0);
}

// Documented in .h file
void TOK_consume(CList tokens)
{
  if (CL_length(tokens) > 0)
    CL_pop(tokens);
}

// Helper function for TOK_print
static void print_token(int pos, CListElementType elem, void *data)
{
  Token tok = elem;
  if (tok.type == TOK_VALUE)
    printf("Token %d: %s(%g)\n", pos, TT_to_str(tok.type), tok.value);
  else
    printf("Token %d: %s\n", pos, TT_to_str(tok.type));
}

// Documented in .h file
void TOK_print(CList tokens)
{
  CL_foreach(tokens, print_token, NULL);
}