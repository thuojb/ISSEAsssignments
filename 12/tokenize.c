// tokenize.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenize.h"

static token_t *create_token(token_type_t type, const char *value) {
    token_t *token = malloc(sizeof(token_t));
    if (!token) return NULL;
    
    token->type = type;
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

static void append_token(token_t **head, token_t *new_token) {
    if (!*head) {
        *head = new_token;
        return;
    }
    
    token_t *current = *head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_token;
}

static int handle_escape(const char *input, int *pos, char *buffer, int *buf_pos) {
    if (input[*pos] != '\\') return 0;
    
    (*pos)++;
    switch (input[*pos]) {
        case 'n': buffer[(*buf_pos)++] = '\n'; break;
        case 'r': buffer[(*buf_pos)++] = '\r'; break;
        case 't': buffer[(*buf_pos)++] = '\t'; break;
        case '"': buffer[(*buf_pos)++] = '"'; break;
        case '\\': buffer[(*buf_pos)++] = '\\'; break;
        case ' ': buffer[(*buf_pos)++] = ' '; break;
        case '|': buffer[(*buf_pos)++] = '|'; break;
        case '<': buffer[(*buf_pos)++] = '<'; break;
        case '>': buffer[(*buf_pos)++] = '>'; break;
        default:
            fprintf(stderr, "Illegal escape character '%c'\n", input[*pos]);
            return -1;
    }
    (*pos)++;
    return 1;
}

token_t *TOK_tokenize_input(const char *input) {
    token_t *head = NULL;
    char buffer[4096];
    int buf_pos = 0;
    int in_quotes = 0;
    
    for (int i = 0; input[i] != '\0';) {
        // Handle whitespace
        if (!in_quotes && isspace(input[i])) {
            if (buf_pos > 0) {
                buffer[buf_pos] = '\0';
                append_token(&head, create_token(TOK_WORD, buffer));
                buf_pos = 0;
            }
            i++;
            continue;
        }
        
        // Handle quotes
        if (input[i] == '"') {
            if (!in_quotes) {
                if (buf_pos > 0) {
                    buffer[buf_pos] = '\0';
                    append_token(&head, create_token(TOK_WORD, buffer));
                    buf_pos = 0;
                }
                in_quotes = 1;
                i++;
                continue;
            } else {
                buffer[buf_pos] = '\0';
                append_token(&head, create_token(TOK_QUOTED_WORD, buffer));
                buf_pos = 0;
                in_quotes = 0;
                i++;
                continue;
            }
        }
        
        // Handle escape sequences
        if (input[i] == '\\') {
            int result = handle_escape(input, &i, buffer, &buf_pos);
            if (result < 0) {
                TOK_free_tokens(head);
                return NULL;
            }
            if (result > 0) continue;
        }
        
        // Handle special characters when not in quotes
        if (!in_quotes) {
            if (input[i] == '<' || input[i] == '>' || input[i] == '|') {
                if (buf_pos > 0) {
                    buffer[buf_pos] = '\0';
                    append_token(&head, create_token(TOK_WORD, buffer));
                    buf_pos = 0;
                }
                
                buffer[0] = input[i];
                buffer[1] = '\0';
                token_type_t type;
                switch (input[i]) {
                    case '<': type = TOK_LESSTHAN; break;
                    case '>': type = TOK_GREATERTHAN; break;
                    case '|': type = TOK_PIPE; break;
                    default: type = TOK_WORD; break;
                }
                append_token(&head, create_token(type, buffer));
                i++;
                continue;
            }
        }
        
        // Add character to buffer
        buffer[buf_pos++] = input[i++];
    }
    
    // Handle unterminated quote
    if (in_quotes) {
        fprintf(stderr, "Unterminated quote\n");
        TOK_free_tokens(head);
        return NULL;
    }
    
    // Handle remaining buffer
    if (buf_pos > 0) {
        buffer[buf_pos] = '\0';
        append_token(&head, create_token(TOK_WORD, buffer));
    }
    
    return head;
}

void TOK_free_tokens(token_t *head) {
    while (head) {
        token_t *temp = head;
        head = head->next;
        free(temp->value);
        free(temp);
    }
}

token_t *TOK_next_consume(token_t **current) {
    if (!current || !*current) return NULL;
    
    token_t *token = *current;
    *current = (*current)->next;
    return token;
}