// ps_test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "token.h"
#include "tokenize.h"
#include "pipeline.h"
#include "parse.h"

// Test tokenization
static void test_tokenization(void) {
    printf("Testing tokenization...\n");
    
    // Test basic tokenization
    token_t *tokens = TOK_tokenize_input("echo hello world");
    assert(tokens != NULL);
    assert(tokens->type == TOK_WORD);
    assert(strcmp(tokens->value, "echo") == 0);
    assert(tokens->next != NULL);
    assert(tokens->next->type == TOK_WORD);
    assert(strcmp(tokens->next->value, "hello") == 0);
    TOK_free_tokens(tokens);
    
    // Test quoted words
    tokens = TOK_tokenize_input("echo \"hello world\"");
    assert(tokens != NULL);
    assert(tokens->type == TOK_WORD);
    assert(tokens->next != NULL);
    assert(tokens->next->type == TOK_QUOTED_WORD);
    assert(strcmp(tokens->next->value, "hello world") == 0);
    TOK_free_tokens(tokens);
    
    // Test redirection
    tokens = TOK_tokenize_input("echo hello > output.txt");
    assert(tokens != NULL);
    assert(tokens->type == TOK_WORD);
    assert(tokens->next != NULL);
    assert(tokens->next->next != NULL);
    assert(tokens->next->next->type == TOK_GREATERTHAN);
    TOK_free_tokens(tokens);
    
    // Test pipes
    tokens = TOK_tokenize_input("echo hello | grep ll");
    assert(tokens != NULL);
    assert(tokens->type == TOK_WORD);
    token_t *pipe_token = tokens->next->next;
    assert(pipe_token != NULL);
    assert(pipe_token->type == TOK_PIPE);
    TOK_free_tokens(tokens);
    
    // Test escape sequences
    tokens = TOK_tokenize_input("echo \"hello\\nworld\"");
    assert(tokens != NULL);
    assert(tokens->next != NULL);
    assert(tokens->next->type == TOK_QUOTED_WORD);
    assert(strchr(tokens->next->value, '\n') != NULL);
    TOK_free_tokens(tokens);
    
    printf("Tokenization tests passed!\n");
}

// Test pipeline creation and manipulation
static void test_pipeline(void) {
    printf("Testing pipeline...\n");
    
    pipeline_t *p = pipeline_create();
    assert(p != NULL);
    
    // Test adding commands
    pipeline_add_command(p);
    pipeline_add_arg(p, "echo");
    pipeline_add_arg(p, "hello");
    
    assert(p->commands != NULL);
    assert(p->commands->argc == 2);
    assert(strcmp(p->commands->argv[0], "echo") == 0);
    assert(strcmp(p->commands->argv[1], "hello") == 0);
    
    // Test adding another command
    pipeline_add_command(p);
    pipeline_add_arg(p, "grep");
    pipeline_add_arg(p, "ll");
    
    assert(p->commands->next != NULL);
    assert(p->commands->next->argc == 2);
    
    pipeline_free(p);
    
    printf("Pipeline tests passed!\n");
}

// Test parsing
static void test_parsing(void) {
    printf("Testing parsing...\n");
    
    // Test basic command
    token_t *tokens = TOK_tokenize_input("echo hello world");
    pipeline_t *p = parse_pipeline(tokens);
    assert(p != NULL);
    assert(p->commands != NULL);
    assert(p->commands->argc == 3);
    TOK_free_tokens(tokens);
    pipeline_free(p);
    
    // Test pipeline with redirection
    tokens = TOK_tokenize_input("echo hello > output.txt");
    p = parse_pipeline(tokens);
    assert(p != NULL);
    assert(p->output != stdout);
    TOK_free_tokens(tokens);
    pipeline_free(p);
    
    // Test multiple commands
    tokens = TOK_tokenize_input("echo hello | grep ll | wc -l");
    p = parse_pipeline(tokens);
    assert(p != NULL);
    assert(p->commands != NULL);
    assert(p->commands->next != NULL);
    assert(p->commands->next->next != NULL);
    TOK_free_tokens(tokens);
    pipeline_free(p);
    
    printf("Parsing tests passed!\n");
}

int main(void) {
    test_tokenization();
    test_pipeline();
    test_parsing();
    
    printf("All tests passed!\n");
    return 0;
}