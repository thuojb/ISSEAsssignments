/*
 * token.h
 * 
 * Defines token types and structures for lexical analysis of shell commands
 *
 * Author: John Bosco Thuo
 */

#ifndef TOKEN_H
#define TOKEN_H

/*
 * Enumeration of possible token types in shell command parsing
 */
typedef enum {
    TOK_WORD,           // Regular word
    TOK_QUOTED_WORD,    // Word in double quotes
    TOK_LESSTHAN,       // < for input redirection
    TOK_GREATERTHAN,    // > for output redirection
    TOK_PIPE           // | for pipeline
} token_type_t;

/*
 * Structure representing a single token in the command line
 */
typedef struct token {
    token_type_t type;  // Type of the token
    char *value;        // Actual token content
    struct token *next; // Next token in linked list
} token_t;

#endif