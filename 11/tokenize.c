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
  case TOK_SYMBOL:
    return "SYMBOL";
  case TOK_EQUAL:
    return "EQUAL";
  case TOK_END:
    return "(end)";
  }

  __builtin_unreachable();
}

// Helper function to validate symbol names
static bool is_valid_symbol_start(char c) {
  return isalpha(c) || c == '_';
}

static bool is_valid_symbol_char(char c) {
  return isalnum(c) || c == '_';
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
      tok.value = strtod(p, &endptr);
      
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
    
    // Handle symbols
    if (is_valid_symbol_start(*p)) {
      const char *symbol_start = p;
      while (is_valid_symbol_char(*p)) p++;
      
      size_t symbol_len = p - symbol_start;
      if (symbol_len > 31) {
        snprintf(errmsg, errmsg_sz, "Symbol too long");
        CL_free(tokens);
        return NULL;
      }
      
      tok.type = TOK_SYMBOL;
      strncpy(tok.symbol, symbol_start, symbol_len);
      tok.symbol[symbol_len] = '\0';
      CL_append(tokens, tok);
      continue;
    }
    
    // Handle operators and parentheses
    if (strchr("+-*/^()", *p)) {
      tok.type = char_to_token_type(*p);
      tok.value = 0;
      CL_append(tokens, tok);
      p++;
      continue;
    }
    
    // Handle assignment
    if (*p == '=') {
      tok.type = TOK_EQUAL;
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

  // Add end token
  tok.type = TOK_END;
  CL_append(tokens, tok);

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