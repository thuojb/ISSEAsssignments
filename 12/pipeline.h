// pipeline.h
#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdio.h>

typedef struct command {
    char **argv;
    int argc;
    int capacity;
    struct command *next;
} command_t;

typedef struct pipeline {
    command_t *commands;
    FILE *input;
    FILE *output;
} pipeline_t;

// Create new pipeline
pipeline_t *pipeline_create(void);

// Add command to pipeline
void pipeline_add_command(pipeline_t *p);

// Add argument to last command
void pipeline_add_arg(pipeline_t *p, const char *arg);

// Set input/output redirection
void pipeline_set_input(pipeline_t *p, FILE *input);
void pipeline_set_output(pipeline_t *p, FILE *output);

// Free pipeline
void pipeline_free(pipeline_t *p);

#endif