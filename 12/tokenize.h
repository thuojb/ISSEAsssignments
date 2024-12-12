// tokenize.h
#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "token.h"

// Tokenize input string into linked list of tokens
token_t *TOK_tokenize_input(const char *input);

// Free token list
void TOK_free_tokens(token_t *head);

// Get next token and advance
token_t *TOK_next_consume(token_t **current);

#endif