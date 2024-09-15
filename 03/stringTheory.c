/*
 * author john bosco thuo
 * isse assignment 03
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

/*
*
* function to check if 'prefix' is a prefix of 'str'
* has a bool type returns true if 'str' starts with 'prefix', false otherwise
* parameters:
*   prefix[], str[]
* returns:
*   bool type
* author: john bosco thuo
*/

bool is_prefix(const char *prefix, const char *str) {
    // if 'prefix' is an empty string, returns true, "" is a prefix of any str ""
    if (*prefix == '\0') {
        return true;
    }

    // if 'str' is an empty string, returns false as no prefix for an empty str
    if (*str == '\0') {
        return false;
    }

    // loop for 'prefix' and 'str' comparison, iteratively character by character
    while (*prefix) {
        // if 'str' is sorts faster or characters don't match, return false
        if (*str == '\0' || *prefix != *str) {
            return false;
        }
        // move to the next characters to merge in the middle
        prefix++;
        str++;
    }

    // If all characters of 'prefix' are matched, return true
    return true;
}


/**
 * the helper function reverse_helper reverses characters in place between two pointers in a string.
 *
 * this function swaps the characters at the start and end pointers, iteratively,
 * moving inward until the pointers meet or cross: start>=end
 *
 * @param start pointer for the beginning of the string to reverse.
 * @param end pointer to the end of the string to reverse.
 */

void reverse_helper(char *start, char *end) {
    while (start < end)
    {
            // swap characters at positions first and last in place with a temp char variable to hold a byte temporarily
            char temp = *start;
            *start = *end;
            *end = temp;

            //traverse from ends to the centre
            start++;
            end--;
    }
}


/**
 * helper function to_lowercase converts a character to lowercase of type unsigned char since,
 * it cannot be negative and this gives a wider range of memory.
 *
 * the function changes the character pointed to by `c` to its lowercase
 * equivalent if it is an uppercase letter.
 *
 * @param c pointer to the character to be convert to lowercase.
 */

void to_lowercase(char *c)
{
        *c = tolower((unsigned char)*c);
}

/**
 * function rever_in_place to reverse the entire string in place with case conversion.
 *
 * the function first converts all characters in the string str to lowercase,
 * and then reverses the string str in place.
 * it combines the functionality of `to_lowercase` and `reverse_helper` to achieve this.
 *
 * @param str pointer to the null-terminated string str to be reverse.
 *
 * @return the function does not return a value, the input string str is modified in place.
 *
 * @details
 * 1. the function starts by converting all characters in the string to lowercase.
 *    - This is done by iterating over each character of str and calling `to_lowercase`.
 * 2. once all characters of str are in lowercase, the function proceeds to reverse the string str in place.
 *    - the `reverse_helper` function is used for this purpose, which swaps characters
 *      from both ends of the string until reaching the center.
 */

void reverse_in_place (char *str)
{

        // pointer to the last character of the string.
        char *end = str + strlen(str) - 1;

        for (char *ptr = str; ptr <= end; ptr++)
        {
                to_lowercase(ptr);
        }

        // to reverse the entire string using the helper function
        reverse_helper(str, end);
}

/**
 * function reverse_by_word to reverse each word in the string str in place.
 * using the `reverse_helper` function to reverse characters within each word.
 * 
 * @param str Pointer to the null-terminated string whose words are to be reversed.
 *
 * @return This function does not return a value. The input string is modified in place.
 *
 * @details
 * 1. the function iterates through the string str character by character.
 * 2. when a whitespace character is encountered, the function checks if a word has been identified.
 *    - if a word is identified (i.e., `new_word` is not NULL), it reverses the word using the helper function `reverse_helper` previously defined.
 *    - after reversing, `new_word` is reset to NULL to start searching the next word.
 * 3. if a non-whitespace character is encountered, and `new_word` is NULL, it marks the beginning of a new word.
 * 4. after the loop completes, the function performs a final check to reverse the last word if `new_word` is still set.
 */

void reverse_by_word(char *str)
{
    char *new_word = NULL;
    char *ptr = str;

    while (*ptr) // traverse the string until the null string terminator: empty byte
    {
        if (isspace(*ptr)) // check for whitespace characters
        {
            if (new_word) { // if a word was identified
                reverse_helper(new_word, ptr - 1); // reverse the word
                new_word = NULL; // reset new_word for the next word
            }
        }
        else {
            if (!new_word) // if a word hasn't been started yet
            {
                new_word = ptr; // mark the start of the new word
            }
        }
        ptr++; // move to the next string str character
    }
    // reverse the last word if the string ended without trailing whitespace
    if (new_word)
    {
        reverse_helper(new_word, ptr - 1);
    }
}

/**
 * function remove_last_substr removes the last occurrence of a substring_prefix from a string.
 *
 * @param str pointer to the null-terminated string to modify.
 * @param substr Pointer to the null-terminated substring_prefix to remove.
 *
 * @return the starting index of the last occurrence of `substr` if found;
 *         otherwise, -1 if `substr` is not found.
 * 
 * @details
 * 1. calculates the lengths of `str` and `substr`.
 * 2. iterates through `str` to find the last occurrence of `substr`.
 *    - for each position in `str`, it uses `is_prefix` to check if `substr`
 *      matches the current position in `str`.
 *    - `is_prefix` checks if `substr` is a prefix of the substr starting
 *      at the current position.
 * 3. if `substr` is found, it removes it by shifting the following characters.
 * 4. returns the index of the last occurrence of `substr`, or -1 if not found.
 */
int remove_last_substr(char *str, const char *substr)
{
    int last_occurrence = -1;
    int str_length = strlen(str);
    int substr_length = strlen(substr);

    // Find the last occurrence of the substring_prefix
    for (int i = 0; i <= str_length - substr_length; i++)
    {
        if (is_prefix(substr, str + i))
        {
            last_occurrence = i;
        }
    }

    // If the substring_prefix is found, remove it
    if (last_occurrence != -1)
    {
        int move_length = str_length - (last_occurrence + substr_length);
        memmove(str + last_occurrence, str + last_occurrence + substr_length, move_length + 1); // +1 to include null terminator

        return last_occurrence;
    }

    return -1;
}

/**
 * helper function to check if a character is in the set of characters to remove.
 *
 * @param c The character to check.
 * @param chars_to_remove Pointer to a string of characters to remove.
 * @return `true` if `c` is in `chars_to_remove`, `false` otherwise.
 */
bool char_in_chars_to_remove(char c, const char *chars_to_remove) {
    while (*chars_to_remove) {
        if (c == *chars_to_remove) {
            return true;
        }
        chars_to_remove++;
    }
    return false;
}

/**
 * functions strip_chars remove all occurrences of characters in `chars_to_remove` from string `str`.
 *
 * @param str pointer to the string to modify.
 * @param chars_to_remove pointer to the string of characters to remove.
 * @return this function does not return a value, string is modified in place.
 *
 * @details
 * uses `char_in_chars_to_remove` to check each character and removes it if present in `chars_to_remove`.
 */
void strip_chars(char *str, const char *chars_to_remove) {
    int read = 0;
    int write = 0;
    int str_length = strlen(str);

    while (read < str_length) {
        char char_to_check = str[read];
        if (!char_in_chars_to_remove(char_to_check, chars_to_remove)) {
            str[write] = char_to_check;
            write++;
        }
        read++;
    }

    str[write] = '\0'; // null-terminate the modified string
}


/*
 * below are a series of test functions.
 * to test each of the above functions.
 * where necessary, a helper function is defined.
 *
 * function test_is_prefix tests the is_prefix function.
 * the function tests various cases to check if the `is_prefix` function performs correctly in identifying
 * whether a given prefix is a prefix of the corresponding string.
 */
void test_is_prefix(void)
{
    const char *prefixes[] = {
        "",
        "C",
        "Carnegie",
        "Carnegie Mellon",
        "Cab",
        "car",
        "C",
        ""
    };
    const char *strings[] = {
        "",
        "Carnegie Mellon",
        "Carnegie Mellon",           
        "Carnegie Mellon",           
        "Carnegie Mellon",           
        "Carnegie Mellon",
        "",
        "Carnegie Mellon"
    };

    bool expected[] = {
        true,
        true,
        true,
        true,
        false,
        false,
        false,
        true
    };

    int num_tests = sizeof(expected) / sizeof(expected[0]);

    printf("Testing is_prefix\n");
    for (int i = 0; i < num_tests; ++i) {
        const char *prefix = prefixes[i];
        const char *str = strings[i];
        bool expected_result = expected[i];

        bool result = is_prefix(prefix, str);
        if (result == expected_result) {
            printf("Test %d passed.\n", i + 1);
        } else {
            printf("Test %d failed: is_prefix(\"%s\", \"%s\") = %s, expected %s\n",
                   i + 1,
                   prefix,
                   str,
                   result ? "true" : "false",
                   expected_result ? "true" : "false");
        }
    }
    printf("Testing is_prefix done\n");
}

/**
 * helper function strings_equal for test cases to compare strings for equality
 * @params chara a and b
 * @param a The first string to compare.
 * @param b The second string to compare.
 *
 * @return true if the strings are equal, false otherwise.
 */

bool strings_equal(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}



/**
 * function test_reverse_in_place tests the reverse_in_place function.
 *
 * the function tests various input strings to ensure that the `reverse_in_place` function
 * correctly reverses the entire string in place after converting all characters to lowercase.
 */
void test_reverse_in_place(void) {
	const char *inputs[] = {
        	"Carnegie Mellon",
		"Four",
		" One",
        	"",
        	"A",
        	"z"
	};
	const char *expected_outputs[] = {
		"nollem eigenrac",
        	"ruof",
        	"eno ",
		"",
        	"a",
        	"z"
	};
	
	int num_tests = sizeof(inputs) / sizeof(inputs[0]);
	
	printf("Testing reverse in place\n");

    	// loop through the test cases
    	for (int i = 0; i < num_tests; ++i) {
        	char str[100]; // create enough buffer for the input string
        	strncpy(str, inputs[i], sizeof(str)); // copy input string to the test buffer
	
        	reverse_in_place(str); // call the function to test

        	// compare the result with the expected output
        	if (strings_equal(str, expected_outputs[i])) {
            		printf("Test %d passed.\n", i + 1);
        	} else {
            		printf("Test %d failed: expected \"%s\", got \"%s\"\n",
					i + 1,
                   			expected_outputs[i],
                   			str);
		}
	}
}

/**
 * function test_reverse_by_word tests the reverse_by_word function.
 *
 * the function tests various strings to ensure that the `reverse_by_word` function
 * correctly reverses each word in the string while maintaining the order of words.
 */
void test_reverse_by_word(void) {
    const char *inputs[] = {
        "Carnegie Mellon",
        "Four",
        " One",
        "",
        "A",
        "z",
        "\tCarnegie  Mellon",
        "Hello\tWorld",
        "  ",
        "abc def  ghi"
    };

    const char *expected_outputs[] = {
        "eigenraC nolleM",
        "ruoF",
        " enO",
        "",
        "A",
        "z",
        "\teigenraC  nolleM",
        "olleH\tdlroW",
        "  ",
        "cba fed  ihg"
    };

    int num_tests = sizeof(inputs) / sizeof(inputs[0]);

    printf("Testing reverse by word\n");
    for (int i = 0; i < num_tests; ++i) {
        char str[100]; // create buffer size is sufficient for the input
        strncpy(str, inputs[i], sizeof(str)); // copy input string to buffer

        reverse_by_word(str);

        if (strings_equal(str, expected_outputs[i])) {
            printf("Test %d passed.\n", i + 1);
        } else {
            printf("Test %d failed: expected \"%s\", got \"%s\"\n",
                   i + 1,
                   expected_outputs[i],
                   str);
        }
    }
}

/**
 * function test_remove_last_substr tests the remove_last_substr function.
 *
 * this function tests the `remove_last_substr` function by running it with various input strings
 * and substrings to verify that the last occurrence of the substring is correctly removed from
 * the initial string. it also checks if the return value matches the expected result.
 */
void test_remove_last_substr() {
    // define test cases
    char *initial_strs[] = {
        "Carnegie Mellon",
        "Carnegie Mellon",
        "Carnegie Mellon",
        "one two one three",
        "one two one three",
        "Bananas"
    };

    char *substrs[] = {
        "Carnegie Mellon",
        "Kiltie",
        "",
        "one ",
        "hr",
	"a"
    };

    char *expected_results[] = {
        "",
	"Carnegie Mellon",
        "Carnegie Mellon",
        "one two one tee",
        "one two one tee",
        "Banans"
    };

    int expected_returns[] = {
        0,
	-1,
        15,
        8,
        13,
        5
    };

    int num_tests = sizeof(expected_returns) / sizeof(expected_returns[0]);

    // looping through the test cases
    for (int i = 0; i < num_tests; i++) {
        char str[100];      // create buffer size is sufficient for the input string
        strcpy(str, initial_strs[i]); // copy input string to the test buffer

        // run the function under test
        int result = remove_last_substr(str, substrs[i]);

        printf("Running test case %d\n", i + 1);
        printf("Initial string: \"%s\"\n", initial_strs[i]);
        printf("Substring: \"%s\"\n", substrs[i]);
        printf("Expected result: \"%s\"\n", expected_results[i]);
        printf("Expected return value: %d\n", expected_returns[i]);
        printf("Function result: %d\n", result);
        printf("Modified string: \"%s\"\n", str);

        // check if the test passed
        if (result == expected_returns[i] && strcmp(str, expected_results[i]) == 0) {
            printf("Test passed.\n\n");
        } else {
            printf("Test failed.\n\n");
        }
    }
}

/**
 * Function to test the strip_chars function.
 *
 * This function verifies that the `strip_chars` function correctly removes all occurrences of
 * specified characters from the input strings. It compares the output of the function with
 * the expected results to ensure correctness.
 */
void test_strip_chars(void) {
    const char *test_strings[] = {
        "Carnegie Mellon",
        "Carnegie Mellon",
        "Carnegie Mellon",
        "one two three",
        "Bananas",
        "Bananas"
    };

    const char *remove_chars_list[] = {
        " l",
        "el",
        "",
        "thre",
        "a",
        "Bnas"
    };

    const char *expected_results[] = {
        "CarnegieMeon",
        "CarngiMon",
        "Carnegie Mellon", 
        "on wo ",
        "Bnns",
        ""
    };

    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    // Loop through each test case
    for (int i = 0; i < num_tests; i++) {
        char str[50];       // Ensure buffer size is sufficient for the input string
        strcpy(str, test_strings[i]); // Copy the input string to the test buffer
        strip_chars(str, remove_chars_list[i]); // Call the function to test

        // Compare the result with the expected output
        if (strcmp(str, expected_results[i]) != 0) {
            printf("strip_chars(\"%s\", \"%s\") = \"%s\"; expected \"%s\"\n",
                   test_strings[i], remove_chars_list[i], str, expected_results[i]);
        } else {
            printf("Test %d passed.\n", i + 1); // Indicate that the test passed
        }
    }
}


int main (int argc, char *argv[])
{
	test_is_prefix();
	printf("All prefix tests done\n");
	test_reverse_in_place();
	printf("All reverse test done!\n");
	test_reverse_by_word();
	printf("All reverse by word tests done!\n");
	test_remove_last_substr();
	printf("All remove last substr tests done\n");
	test_strip_chars();
	printf("All strip char tests done\n");
	return 0;
}
