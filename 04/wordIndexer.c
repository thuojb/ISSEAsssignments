#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common_words.h" // Include your common words header

#define MAX_UNIQUE_WORDS 1200
#define MAX_WORD_LENGTH 21
#define MAX_LINE_LENGTH 255
#define MAX_LINE_NUMBERS 20

typedef struct {
    char word[MAX_WORD_LENGTH];
    int total_occurrences; // Total occurrences of the word across all lines
    int line_numbers[MAX_LINE_NUMBERS]; // Lines where the word appears
} WordIndex;

// Function prototypes
void read_input(WordIndex *index, int *word_count, FILE *fin);
int add_word(WordIndex *index, int *word_count, const char *word, int line_num);
void sort_and_print(WordIndex *index, int word_count);
int is_common_word(const char *word);

void read_input(WordIndex *index, int *word_count, FILE *fin) {
    char line[MAX_LINE_LENGTH];
    int line_num = 1;

    while (fgets(line, sizeof(line), fin)) {
        if (strlen(line) >= MAX_LINE_LENGTH) {
            fprintf(stderr, "Line exceeded maximum length\n");
            exit(1);
        }

        char *ptr = line;
        char word[MAX_WORD_LENGTH];
        int word_len = 0;

        while (*ptr) {
            if (isalpha(*ptr) || (*ptr == '\'' && word_len > 0) || (*ptr == '-' && word_len > 0)) {
                if (word_len < MAX_WORD_LENGTH - 1) {
                    word[word_len++] = tolower(*ptr);
                }
	    }
	    else 
	    {
                if (word_len > 0) {
                    word[word_len] = '\0';

                if (strlen(word) > MAX_WORD_LENGTH - 1) {
                    fprintf(stderr, "Word '%s' exceeds maximum length of 20 characters\n", word);
                    exit(1);
                }

                if (!is_common_word(word)) {
                    if (add_word(index, word_count, word, line_num) == 1) {
                        fprintf(stderr, "Exceeded unique words limit\n");
                        exit(1);
		    }
                }
                word_len = 0; // Reset for next word
         
    		}
            ptr++;
        }

        if (word_len > 0) { // Moved outside the previous conditional
            word[word_len] = '\0'; // Ensure to null-terminate
            if (strlen(word) > MAX_WORD_LENGTH - 1) {
                fprintf(stderr, "Word '%s' exceeds maximum length of 20 characters\n", word);
                exit(1);
            }

            if (!is_common_word(word)) {
                if (add_word(index, word_count, word, line_num) == -1) {
                    fprintf(stderr, "Exceeded unique words limit\n");
                    exit(1);
                }
            }
        }
        line_num++;
    }
}

int add_word(WordIndex *index, int *word_count, const char *word, int line_num) {
    for (int i = 0; i < *word_count; i++) {
        if (strcmp(index[i].word, word) == 0) {
            // Check if the word has been recorded in more than 20 lines
            int line_count = 0;
            for (int j = 0; j < MAX_LINE_NUMBERS; j++) {
                if (index[i].line_numbers[j] != 0) {
                    line_count++;
                }
            }
            if (line_count >= MAX_LINE_NUMBERS) {
                fprintf(stderr, "Word '%s' exceeds maximum line occurrences\n", word);
                exit(1); // Exit if it exceeds 20 lines
            }

            index[i].total_occurrences++; // Increment total occurrences

            // Check if this line number has already been recorded
            for (int j = 0; j < MAX_LINE_NUMBERS; j++) {
                if (index[i].line_numbers[j] == line_num) {
                    return 0; // Word already recorded for this line
                }
            }

            // Add this line number since it wasn't recorded before
            for (int j = 0; j < MAX_LINE_NUMBERS; j++) {
                if (index[i].line_numbers[j] == 0) { // Find the first empty spot
                    index[i].line_numbers[j] = line_num; // Add line number
                    break; // Stop searching after adding
                }
            }
            return 0;
        }
    }

    // Add new word if not found
    if (*word_count < MAX_UNIQUE_WORDS) {
        strcpy(index[*word_count].word, word);
        index[*word_count].total_occurrences = 1; // First occurrence
        index[*word_count].line_numbers[0] = line_num; // Record line number
        (*word_count)++;
        return 0;
    }
    fprintf(stderr, "Exceeding unique word limit\n");
    exit(1); // Unique words limit exceeded
}

void sort_and_print(WordIndex *index, int word_count) {
    // Sort using a simple bubble sort
    for (int i = 0; i < word_count - 1; i++) {
        for (int j = i + 1; j < word_count; j++) {
            if (index[i].total_occurrences < index[j].total_occurrences ||
                (index[i].total_occurrences == index[j].total_occurrences && strcmp(index[i].word, index[j].word) > 0)) {
                WordIndex temp = index[i];
                index[i] = index[j];
                index[j] = temp;
            }
        }
    }

    // Print sorted output
    for (int i = 0; i < word_count; i++) {
        printf("%s [%d]: ", index[i].word, index[i].total_occurrences); // Use total occurrences
        for (int j = 0; j < MAX_LINE_NUMBERS; j++) {
            if (index[i].line_numbers[j] != 0) {
                printf("%d", index[i].line_numbers[j]);
                if (j < MAX_LINE_NUMBERS - 1) {
                    printf(", ");
                }
            }
        }
        printf("\n");
    }
}

int is_common_word(const char *word) {
    for (int i = 0; common_words[i] != NULL; i++) {
        if (strcmp(word, common_words[i]) == 0) {
            return 1; // Word is common
        }
    }
    return 0; // Word is not common
}

int main(int argc, char **argv) {
    WordIndex index[MAX_UNIQUE_WORDS] = {0};
    int word_count = 0;

    // Read input from stdin
    read_input(index, &word_count, stdin);

    if (word_count == 0) {
        fprintf(stderr, "No words found\n");
        return 1;
    }

    sort_and_print(index, word_count);
    return 0;
}

