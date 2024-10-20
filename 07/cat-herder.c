#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

// function to create modified environment for kitty -4 (limited environment)
char **create_limited_env(char **env) {
    char *path = NULL;
    char *home = NULL;
    
    // finding PATH and HOME from parent environment
    for (int i = 0; env[i] != NULL; i++) {
        if (strncmp(env[i], "PATH=", 5) == 0) {
            path = strdup(env[i]);
        } else if (strncmp(env[i], "HOME=", 5) == 0) {
            home = strdup(env[i]);
        }
    }
    
    // creating new environment with exactly 3 variables
    char **new_env = malloc(4 * sizeof(char *));
    if (!new_env) {
        perror("malloc failed");
        exit(1);
    }
    
    // kitty checking for exactly these three variables with order constraint
    new_env[0] = path ? path : strdup("PATH=/bin:/usr/bin");
    new_env[1] = home ? home : strdup("HOME=/tmp");
    new_env[2] = strdup("CATFOOD=yummy");
    new_env[3] = NULL;
    
    return new_env;
}

// function to create environment without KITTYLITTER for kitty -3
char **create_filtered_env(char **env) {
    // count environment variables
    int count = 0;
    while (env[count] != NULL) count++;
    
    char **new_env = malloc((count + 1) * sizeof(char *));
    if (!new_env) {
        perror("malloc failed");
        exit(1);
    }
    
    int j = 0;
    for (int i = 0; env[i] != NULL; i++) {
        if (strncmp(env[i], "KITTYLITTER=", 12) != 0) {
            new_env[j++] = strdup(env[i]);
        }
    }
    new_env[j] = NULL;
    
    return new_env;
}

// function to add CATFOOD to environment for kitty -2
char **create_enhanced_env(char **env) {
    int count = 0;
    while (env[count] != NULL) count++;
    
    char **new_env = malloc((count + 2) * sizeof(char *));
    if (!new_env) {
        perror("malloc failed");
        exit(1);
    }
    
    for (int i = 0; env[i] != NULL; i++) {
        new_env[i] = strdup(env[i]);
    }
    new_env[count] = strdup("CATFOOD=yummy");
    new_env[count + 1] = NULL;
    
    return new_env;
}

// clean up environment array
void free_env(char **env) {
    if (env) {
        for (int i = 0; env[i] != NULL; i++) {
            free(env[i]);
        }
        free(env);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s inputfile outputfile\n", argv[0]);
        exit(1);
    }
    
    // check if input and output files are the same
    if (strcmp(argv[1], argv[2]) == 0) {
        fprintf(stderr, "Error: Input and output files must be different\n");
        exit(1);
    }
    
    // create pipes for communication between processes
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("pipe failed");
        exit(1);
    }
    
    // first child (kitty -2)
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid1 == 0) {  // first child
        // close unused pipe ends
        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        // open input file
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("open input failed");
            exit(1);
        }
        
        // redirect stdin and stdout
        dup2(fd, STDIN_FILENO);
        dup2(pipe1[1], STDOUT_FILENO);
        close(fd);
        close(pipe1[1]);
        
        // modifying environment - add CATFOOD=yummy
        char **new_env = create_enhanced_env(envp);
        
        execle("/var/local/isse-07/kitty", "kitty", "-2", NULL, new_env);
        perror("execle failed");
        free_env(new_env);
        exit(1);
    }
    
    // second child (kitty -3)
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid2 == 0) {  // second child
        // close unused pipe ends
        close(pipe1[1]);
        close(pipe2[0]);
        
        // redirect stdin and stdout
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe2[1]);
        
        // creating environment without KITTYLITTER
        char **new_env = create_filtered_env(envp);
        
        execle("/var/local/isse-07/kitty", "kitty", "-3", NULL, new_env);
        perror("execle failed");
        free_env(new_env);
        exit(1);
    }
    
    // third child (kitty -4)
    pid_t pid3 = fork();
    if (pid3 < 0) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid3 == 0) {  // third child
        // close unused pipe ends
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        
        // open output file
        int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("open output failed");
            exit(1);
        }
        
        // redirect stdin and stdout
        dup2(pipe2[0], STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
        close(pipe2[0]);
        
        // create limited environment with exactly PATH, HOME, and CATFOOD
        char **new_env = create_limited_env(envp);
        
        execle("/var/local/isse-07/kitty", "kitty", "-4", NULL, new_env);
        perror("execle failed");
        free_env(new_env);
        exit(1);
    }
    
    // parent process
    // close all pipe ends in parent
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    // wait for all children
    int status1, status2, status3;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    waitpid(pid3, &status3, 0);
    
    // check exit status of all children
    if (WIFEXITED(status1) && WEXITSTATUS(status1) == 0 &&
        WIFEXITED(status2) && WEXITSTATUS(status2) == 0 &&
        WIFEXITED(status3) && WEXITSTATUS(status3) == 0) {
        exit(0);
    } else {
        exit(1);
    }
}