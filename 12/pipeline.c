/*
 * pipeline.c
 * 
 * Implementation of command pipeline management
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pipeline.h"

#define INITIAL_CAPACITY 8

/*
 * Creates a new command structure
 *
 * Returns: Pointer to new command or NULL on error
 */

static command_t *command_create(void) {
    command_t *cmd = malloc(sizeof(command_t));
    if (!cmd) return NULL;
    
    cmd->argv = malloc(INITIAL_CAPACITY * sizeof(char*));
    if (!cmd->argv) {
        free(cmd);
        return NULL;
    }
    
    cmd->argc = 0;
    cmd->capacity = INITIAL_CAPACITY;
    cmd->next = NULL;
    return cmd;
}

/*
 * Creates a new command structure
 *
 * Returns: Pointer to new command or NULL on error
 */

static void command_free(command_t *cmd) {
    if (!cmd) return;
    
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);
    free(cmd);
}

/*
 * Creates a new pipeline structure
 *
 * Returns: Pointer to new pipeline structure, or NULL on allocation failure
 */

pipeline_t *pipeline_create(void) {
    pipeline_t *p = malloc(sizeof(pipeline_t));
    if (!p) return NULL;
    
    p->commands = NULL;
    p->input = stdin;
    p->output = stdout;
    return p;
}

/*
 * Adds a new command to the end of the pipeline
 *
 * Parameters:
 *   p        Pipeline to add command to
 */

void pipeline_add_command(pipeline_t *p) {
    if (!p) return;
    
    command_t *cmd = command_create();
    if (!cmd) return;
    
    if (!p->commands) {
        p->commands = cmd;
        return;
    }
    
    command_t *current = p->commands;
    while (current->next) {
        current = current->next;
    }
    current->next = cmd;
}

/*
 * Adds an argument to the most recently added command
 *
 * Parameters:
 *   p        Pipeline containing the command
 *   arg      Argument string to add
 *
 * Notes:
 *   - Creates new command if pipeline is empty
 *   - Automatically resizes argument array if needed
 *   - Maintains NULL termination of argv array
 */

void pipeline_add_arg(pipeline_t *p, const char *arg) {
    if (!p || !arg) return;
    
    command_t *cmd = p->commands;
    if (!cmd) {
        pipeline_add_command(p);
        cmd = p->commands;
    } else {
        while (cmd->next) {
            cmd = cmd->next;
        }
    }
    
    // Ensure space for new arg and NULL terminator
    if (cmd->argc + 1 >= cmd->capacity) {
        int new_capacity = cmd->capacity * 2;
        char **new_argv = realloc(cmd->argv, new_capacity * sizeof(char*));
        if (!new_argv) return;
        
        cmd->argv = new_argv;
        cmd->capacity = new_capacity;
    }
    
    cmd->argv[cmd->argc] = strdup(arg);
    if (!cmd->argv[cmd->argc]) return;
    
    cmd->argc++;
    cmd->argv[cmd->argc] = NULL; // Keep NULL-terminated
}

/*
 * Sets the input stream for the pipeline
 *
 * Parameters:
 *   p        Pipeline to modify
 *   input    Input file stream to use
 */

void pipeline_set_input(pipeline_t *p, FILE *input) {
    if (!p) return;
    if (p->input != stdin) {
        fclose(p->input);
    }
    p->input = input;
}

/*
 * Sets the output stream for the pipeline
 *
 * Parameters:
 *   p        Pipeline to modify
 *   output   Output file stream to use
 */

void pipeline_set_output(pipeline_t *p, FILE *output) {
    if (!p) return;
    if (p->output != stdout) {
        fclose(p->output);
    }
    p->output = output;
}

/*
 * Frees all memory associated with a pipeline
 *
 * Parameters:
 *   p        Pipeline to free
 *
 * Notes:
 *   - Closes redirected input/output streams
 *   - Does not close stdin/stdout
 */

void pipeline_free(pipeline_t *p) {
    if (!p) return;
    
    command_t *current = p->commands;
    while (current) {
        command_t *next = current->next;
        command_free(current);
        current = next;
    }
    
    // Don't close stdin/stdout
    if (p->input != stdin) {
        fclose(p->input);
    }
    if (p->output != stdout) {
        fclose(p->output);
    }
    
    free(p);
}