/*
 * parse.c
 * 
 * Implementation of shell command parsing functionality
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include "parse.h"
#include "pipeline.h"
#include "token.h"

/*
 * Handles input/output redirection tokens
 *
 * Parameters:
 *   pipeline    Pipeline to configure redirection for
 *   current     Pointer to current token pointer
 *
 * Returns: 0 on success, -1 on error
 */

static int handle_redirection(pipeline_t *pipeline, token_t **current) {
    token_t *token = *current;
    token_t *next = token->next;
    
    if (!next || next->type != TOK_WORD) {
        fprintf(stderr, "Expect filename after redirection\n");
        return -1;
    }
    
    if (token->type == TOK_LESSTHAN) {
        if (pipeline->input != stdin) {
            fprintf(stderr, "Multiple redirection\n");
            return -1;
        }
        FILE *file = fopen(next->value, "r");
        if (!file) {
            perror(next->value);
            return -1;
        }
        pipeline_set_input(pipeline, file);
    } else { // TOK_GREATERTHAN
        if (pipeline->output != stdout) {
            fprintf(stderr, "Multiple redirection\n");
            return -1;
        }
        FILE *file = fopen(next->value, "w");
        if (!file) {
            perror(next->value);
            return -1;
        }
        pipeline_set_output(pipeline, file);
    }
    
    *current = next->next;
    return 0;
}

/*
 * Expands glob patterns into matching filenames
 *
 * Parameters:
 *   pipeline    Pipeline to add expanded filenames to
 *   pattern     Glob pattern to expand
 *
 * Returns: 0 on success, -1 on error
 */

int expand_glob(pipeline_t *pipeline, const char *pattern) {
    glob_t globbuf;
    int flags = GLOB_TILDE_CHECK;
    
    int ret = glob(pattern, flags, NULL, &globbuf);
    if (ret != 0) {
        // No matches found or error
        if (ret == GLOB_NOMATCH) {
            // If no matches, use the pattern as-is
            pipeline_add_arg(pipeline, pattern);
            return 0;
        }
        return -1;
    }
    
    // Add all matches to the pipeline
    for (size_t i = 0; i < globbuf.gl_pathc; i++) {
        pipeline_add_arg(pipeline, globbuf.gl_pathv[i]);
    }
    
    globfree(&globbuf);
    return 0;
}

/*
 * Parses token stream into pipeline structure
 *
 * Parameters:
 *   tokens      Head of token linked list
 *
 * Returns: Constructed pipeline or NULL on error
 */

pipeline_t *parse_pipeline(token_t *tokens) {
    pipeline_t *pipeline = pipeline_create();
    if (!pipeline) return NULL;
    
    token_t *current = tokens;
    
    // Check for pipe at start
    if (current && current->type == TOK_PIPE) {
        fprintf(stderr, "No command specified\n");
        pipeline_free(pipeline);
        return NULL;
    }
    
    while (current) {
        if (current->type == TOK_PIPE) {
            if (!current->next || current->next->type == TOK_PIPE) {
                fprintf(stderr, "No command specified\n");
                pipeline_free(pipeline);
                return NULL;
            }
            pipeline_add_command(pipeline);
            current = current->next;
            continue;
        }
        
        // Handle redirection
        if (current->type == TOK_LESSTHAN || current->type == TOK_GREATERTHAN) {
            if (handle_redirection(pipeline, &current) < 0) {
                pipeline_free(pipeline);
                return NULL;
            }
            continue;
        }
        
        // Handle word tokens
        if (current->type == TOK_WORD) {
            if (!pipeline->commands) {
                pipeline_add_command(pipeline);
            }
            // Use expand_glob for regular words
            if (strchr(current->value, '*') || strchr(current->value, '?') ||
                strchr(current->value, '[') || strchr(current->value, '~')) {
                expand_glob(pipeline, current->value);
            } else {
                pipeline_add_arg(pipeline, current->value);
            }
            current = current->next;
            continue;
        } else if (current->type == TOK_QUOTED_WORD) {
            if (!pipeline->commands) {
                pipeline_add_command(pipeline);
            }
            pipeline_add_arg(pipeline, current->value);
            current = current->next;
            continue;
        }
        
        // Unknown token type
        fprintf(stderr, "Syntax error\n");
        pipeline_free(pipeline);
        return NULL;
    }
    
    // Check if we have at least one command
    if (!pipeline->commands) {
        pipeline_free(pipeline);
        return NULL;
    }
    
    return pipeline;
}