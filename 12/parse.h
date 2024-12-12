// parse.h
#ifndef PARSE_H
#define PARSE_H

#include "token.h"
#include "pipeline.h"

// Parse tokens into a pipeline structure
pipeline_t *parse_pipeline(token_t *tokens);

// Expand glob patterns
int expand_glob(pipeline_t *pipeline, const char *pattern);

#endif