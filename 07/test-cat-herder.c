#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <assert.h>

#define TEST_INPUT_FILE "test_input.txt"
#define TEST_OUTPUT_FILE "test_output.txt"
#define DIFFERENT_OUTPUT_FILE "different_output.txt"
#define KITTY_PATH "/var/local/isse-07/kitty"

// utility function to create a test file with content
void create_test_file(const char *filename, const char *content) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Failed to create test file");
        exit(1);
    }
    fprintf(f, "%s", content);
    fclose(f);
}

// utility function to compare file contents
int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    if (!f1 || !f2) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }
    
    int ch1, ch2;
    while ((ch1 = fgetc(f1)) != EOF && (ch2 = fgetc(f2)) != EOF) {
        if (ch1 != ch2) {
            fclose(f1);
            fclose(f2);
            return 0;
        }
    }
    
    int result = (fgetc(f1) == EOF && fgetc(f2) == EOF);
    fclose(f1);
    fclose(f2);
    return result;
}

// utility function to check if a file exists
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// test cases
void test_basic_functionality() {
    printf("Testing basic functionality... ");
    
    // create test input file
    create_test_file(TEST_INPUT_FILE, "Hello, World!\n");
    
    // run cat-herder
    char *args[] = {"./cat-herder", TEST_INPUT_FILE, TEST_OUTPUT_FILE, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execv("./cat-herder", args);
        exit(1);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // check exit status
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    
    // verify output file exists and contains correct content
    assert(file_exists(TEST_OUTPUT_FILE));
    assert(compare_files(TEST_INPUT_FILE, TEST_OUTPUT_FILE));
    
    printf("PASS\n");
}

void test_same_file_error() {
    printf("Testing same input/output file... ");
    
    // run cat-herder with same input and output file
    char *args[] = {"./cat-herder", TEST_INPUT_FILE, TEST_INPUT_FILE, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execv("./cat-herder", args);
        exit(0);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // should exit with error
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
    
    printf("PASS\n");
}

void test_environment_variables() {
    printf("Testing environment variables... ");
    
    // test with KITTYLITTER set
    setenv("KITTYLITTER", "dirty", 1);
    
    char *args[] = {"./cat-herder", TEST_INPUT_FILE, DIFFERENT_OUTPUT_FILE, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execv("./cat-herder", args);
        exit(0);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // should still succeed despite KITTYLITTER being set in parent
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    
    // clean up
    unsetenv("KITTYLITTER");
    
    printf("PASS\n");
}

void test_large_file() {
    printf("Testing with large file... ");
    
    // create large test file (1MB)
    FILE *f = fopen(TEST_INPUT_FILE, "w");
    assert(f != NULL);
    for (int i = 0; i < 1024 * 1024; i++) {
        fputc('A' + (i % 26), f);
    }
    fclose(f);
    
    // run cat-herder
    char *args[] = {"./cat-herder", TEST_INPUT_FILE, TEST_OUTPUT_FILE, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execv("./cat-herder", args);
        exit(0);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // verify success and file contents
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    assert(compare_files(TEST_INPUT_FILE, TEST_OUTPUT_FILE));
    
    printf("PASS\n");
}

void test_nonexistent_input() {
    printf("Testing with nonexistent input file... ");
    
    char *args[] = {"./cat-herder", "nonexistent.txt", TEST_OUTPUT_FILE, NULL};
    pid_t pid = fork();
    if (pid == 0) {
        execv("./cat-herder", args);
        exit(0);
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    // should fail
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
    
    printf("PASS\n");
}

void test_invalid_arguments() {
    printf("Testing with invalid arguments... ");
    
    // test with no arguments
    char *args1[] = {"./cat-herder", NULL};
    pid_t pid1 = fork();
    if (pid1 == 0) {
        execv("./cat-herder", args1);
        exit(0);
    }
    
    int status;
    waitpid(pid1, &status, 0);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
    
    // test with one argument
    char *args2[] = {"./cat-herder", TEST_INPUT_FILE, NULL};
    pid_t pid2 = fork();
    if (pid2 == 0) {
        execv("./cat-herder", args2);
        exit(0);
    }
    
    waitpid(pid2, &status, 0);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 1);
    
    printf("PASS\n");
}

void cleanup() {
    unlink(TEST_INPUT_FILE);
    unlink(TEST_OUTPUT_FILE);
    unlink(DIFFERENT_OUTPUT_FILE);
}

int main() {
    // verify kitty exists
    if (access(KITTY_PATH, X_OK) != 0) {
        fprintf(stderr, "Error: %s not found or not executable\n", KITTY_PATH);
        return 1;
    }
    
    printf("Running cat-herder tests...\n\n");
    
    test_basic_functionality();
    test_same_file_error();
    test_environment_variables();
    test_large_file();
    test_nonexistent_input();
    test_invalid_arguments();
    
    cleanup();
    
    printf("\nAll tests passed!\n");
    return 0;
}