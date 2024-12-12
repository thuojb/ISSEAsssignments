/*
 * plaidsh.c
 * 
 * Main shell implementation with command execution
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "token.h"
#include "tokenize.h"
#include "pipeline.h"
#include "parse.h"

#define PROMPT "\001\033[01;31m\002#?\001\033[00;39m\002 "

// Built-in command function prototypes
static int builtin_cd(char **argv);
static int builtin_pwd(pipeline_t *pipeline);
static int builtin_author(pipeline_t *pipeline);
static int handle_path_variable(command_t *cmd, pipeline_t *pipeline);

// Execute a single command in the pipeline
static int execute_command(command_t *cmd, int in_fd, int out_fd) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) { // Child process
        // Handle input redirection
        if (in_fd != STDIN_FILENO) {
            if (dup2(in_fd, STDIN_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(in_fd);
        }
        
        // Handle output redirection
        if (out_fd != STDOUT_FILENO) {
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(1);
            }
            close(out_fd);
        }
        
        // If it's a shell script, execute with bash
        if (strstr(cmd->argv[0], ".sh")) {
            char **new_argv = malloc((cmd->argc + 2) * sizeof(char*));
            if (!new_argv) {
                perror("malloc");
                exit(1);
            }
            new_argv[0] = "/bin/bash";
            for (int i = 0; i < cmd->argc; i++) {
                new_argv[i + 1] = cmd->argv[i];
            }
            new_argv[cmd->argc + 1] = NULL;
            
            execv("/bin/bash", new_argv);
            free(new_argv);
            perror("execv");
            exit(1);
        }
        
        // Execute regular command
        execvp(cmd->argv[0], cmd->argv);
        fprintf(stderr, "%s: Command not found\n", cmd->argv[0]);
        exit(2);
    }
    
    return pid;
}

/*
 * Handles special $PATH variable
 *
 * Parameters:
 *   cmd     Command to check for $PATH
 *
 * Returns: 1 if handled, 0 if not handled
 */
static int handle_path_variable(command_t *cmd, pipeline_t *pipeline) {
    if (cmd->argc == 1 && strcmp(cmd->argv[0], "$PATH") == 0) {
        char *path = getenv("PATH");
        if (path) {
            fprintf(pipeline->output, "%s\n", path);
            return 1;
        }
    }
    return 0;
}

// Execute the entire pipeline
static void execute_pipeline(pipeline_t *pipeline) {
    if (!pipeline || !pipeline->commands) return;
    
    command_t *cmd = pipeline->commands;
    
    // Handle built-in commands first
    if (!cmd->next) { // Single command
        // Check for $PATH first
        if (handle_path_variable(cmd, pipeline)) {
            return;
        }
        
        // Then check other built-ins
        if (strcmp(cmd->argv[0], "cd") == 0) {
            builtin_cd(cmd->argv);
            return;
        } else if (strcmp(cmd->argv[0], "pwd") == 0) {
            builtin_pwd(pipeline);
            return;
        } else if (strcmp(cmd->argv[0], "author") == 0) {
            builtin_author(pipeline);
            return;
        } else if (strcmp(cmd->argv[0], "exit") == 0 ||
                   strcmp(cmd->argv[0], "quit") == 0) {
            exit(0);
        }
    }

        int in_fd = fileno(pipeline->input);
    int pipefds[2];
    pid_t *pids = malloc(sizeof(pid_t) * 100); // Support up to 100 commands
    if (!pids) {
        perror("malloc");
        return;
    }
    int pid_count = 0;
    
    // Execute each command in the pipeline
    while (cmd) {
        int out_fd;
        
        if (cmd->next) {
            // Create pipe for all but last command
            if (pipe(pipefds) < 0) {
                perror("pipe");
                free(pids);
                return;
            }
            out_fd = pipefds[1];
        } else {
            // Last command uses pipeline output
            out_fd = fileno(pipeline->output);
        }
        
        // Execute command
        pid_t pid = execute_command(cmd, in_fd, out_fd);
        if (pid > 0) {
            pids[pid_count++] = pid;
        }
        
        // Close used file descriptors
        if (in_fd != STDIN_FILENO) {
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            close(out_fd);
        }
        
        // Set up for next command
        if (cmd->next) {
            in_fd = pipefds[0];
        }
        cmd = cmd->next;
    }
    
    // Wait for all child processes
    for (int i = 0; i < pid_count; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child %d exited with status %d\n", 
                    (int)pids[i], WEXITSTATUS(status));
        }
    }
    
    free(pids);
}

// Built-in cd command
static int builtin_cd(char **argv) {
    char *dir = argv[1];
    if (!dir) {
        dir = getenv("HOME");
    }
    
    if (chdir(dir) < 0) {
        perror(dir);
        return 1;
    }
    return 0;
}

// Built-in pwd command
static int builtin_pwd(pipeline_t *pipeline) {
    char cwd[4096];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return 1;
    }
    
    fprintf(pipeline->output, "%s\n", cwd);
    return 0;
}

// Built-in author command
static int builtin_author(pipeline_t *pipeline) {
    fprintf(pipeline->output, "John Bosco Thuo\n"); //
    return 0;
}

// Initialize readline with custom completion
static void initialize_readline(void) {
    rl_bind_key('\t', rl_complete);
    using_history();
}

int main(void) {
    char *line;
    
    initialize_readline();
    printf("Welcome to Plaid Shell!\n");
    
    while ((line = readline(PROMPT)) != NULL) {
        if (strlen(line) > 0) {
            add_history(line);
            
            // Tokenize input
            token_t *tokens = TOK_tokenize_input(line);
            if (tokens == NULL) {
                free(line);
                continue;
            }
            
            // Parse tokens into pipeline
            pipeline_t *pipeline = parse_pipeline(tokens);
            if (pipeline == NULL) {
                TOK_free_tokens(tokens);
                free(line);
                continue;
            }
            
            // Execute pipeline
            execute_pipeline(pipeline);
            
            // Cleanup
            pipeline_free(pipeline);
            TOK_free_tokens(tokens);
        }
        free(line);
    }
    
    printf("\n");
    return 0;
}
