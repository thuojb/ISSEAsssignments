/*
 *
 *
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


bool is_prefix(const char prefix[], const char str[])
{
	if (prefix[0] == '\0') { //handles null prefix: empty prefix is a prefix to anythin
		return true;
	}

	if  (str[0]== '\0') { //handles null string
		return false;
	}

	int i = 0;

	// iterate through the arrays while prefix is not null
	while (prefix[i])	// remove the null prefix and retest
	{
		if (str[i] == '\0' || prefix[i] != str[i] ) {
			return false;
		}
		i++;
	}
	return true;
}


// Helper function to reverse characters in place between two pointers
void reverse_helper(char *start, char *end) {
    while (start < end) 
    {
	    // Swap characters at positions first and last
	    char temp = *start;
	    *start = *end;
	    *end = temp;
	    
	    //traverse from ends to the centre
	    start++;
	    end--;
    }
}

// Helper funtion to convert character to lower case
void to_lowercase(char *c)
{
	*c = tolower((unsigned char)*c);
}

// Function to reverse the entire string in place with case conversion
void reverse_in_place (char *str)
{

	char *end = str + strlen(str) - 1;

	for (char *ptr = str; ptr <= end; ptr++)
	{
		to_lowercase(ptr);
	}

	// to reverse the entire string using the helper function
	reverse_helper(str, end);
}



// Helper function for test cases to compate strings
bool strings_equal(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}


void reverse_by_word(char *str)
{
	char *word_start = NULL;
	char *ptr = str;

	while (*ptr) // Check while (*ptr != '\0')
	{
		if (isspace(*ptr))
	       {
		       if (word_start) {
			       reverse_helper(word_start, ptr -1);
			       word_start = NULL;
		       }
	       }
		else {
			if (!word_start)
			{
				word_start = ptr;
			}
		}
		ptr++;
	}
	// reverse the last word in need be
	if (word_start)
	{
		reverse_helper(word_start, ptr -1);
	}
}



// Function to check if a character is in remove_chars
// Function to check if `substr` is a prefix of `str`
// Function to remove the last occurrence of `substr` from `str`
int remove_last_substr(char *str, const char *substr)
{
    int last_occurrence = -1;
    int str_length = strlen(str);
    int substr_length = strlen(substr);

    // Find the last occurrence of the substring
    for (int i = 0; i <= str_length - substr_length; i++)
    {
        if (is_prefix(substr, str + i))
        {
            last_occurrence = i;
        }
    }

    // If we found the substring
    if (last_occurrence != -1)
    {
        // Shift characters to remove the last occurrence
        int move_length = str_length - (last_occurrence + substr_length);
        for (int i = 0; i <= move_length; i++)
        {
            str[last_occurrence + i] = str[last_occurrence + substr_length + i];
        }

        // Null-terminate the modified string
        str[str_length - substr_length] = '\0';

        return last_occurrence;
    }

    return -1;
}


// Helper Function
// Function to check if a character is in the set of characters to remove
bool char_in_remove_chars(char c, const char *remove_chars) {
    while (*remove_chars) {
        if (c == *remove_chars) {
            return true;
        }
        remove_chars++;
    }
    return false;
}

// Function to remove all occurrences of characters in remove_chars from str
void strip_chars(char *str, const char *remove_chars) {
    int read = 0;
    int write = 0;
    int str_length = strlen(str);

    while (read < str_length) {
        char char_to_check = str[read];
        if (!char_in_remove_chars(char_to_check, remove_chars)) {
            str[write] = char_to_check;
            write++;
        }
        read++;
    }

    str[write] = '\0'; // Null-terminate the modified string
}





void test_is_prefix (void)
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
		true,	// null is prefix to null
		true,	// "C" is a prefix of "Carnegie Mellon"	     
		true,	// "Carnegie" is a prefix of "Carnegie Mellon"
		true,	// "Carnegie Mellon" is a prefix of "Carnegie Mellon"
		false,	// "Cab" is not a prefix of "Carnegie Mellon"
		false,	// "car" (case sensitive) is not a prefix of "Carnegie Mellon"
		false,	// an empty string cannot have a prefix that is not empty
		true	// empty "" is a prefix to anything
	};

	int num_tests = sizeof(expected) / sizeof(expected[0]);
	
	printf("Testing is_prefix\n");
	for (int i = 0; i < num_tests; ++i) {
        // Access the current test case
		const char *prefix = prefixes[i];
		const char *str = strings[i];
		bool expected_result = expected[i];

		// Call the function and check result
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







// Test cases function
void test_reverse_in_place(void) {
	// Define the test cases
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

	// Loop through the test cases
	printf("Testing reverse in place\n");
	for (int i = 0; i < num_tests; ++i) {
		char str[100]; // Make sure this size is large enough for the test cases
		strncpy(str, inputs[i], sizeof(str)); // Copy the input string to the test buffer

		reverse_in_place(str);
		
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




// Test cases function
void test_reverse_by_word(void) {
    // Define the test cases
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

    // Loop through the test cases
    for (int i = 0; i < num_tests; ++i) {
        char str[100]; // Make sure this size is large enough for the test cases
        strncpy(str, inputs[i], sizeof(str)); // Copy the input string to the test buffer

        reverse_by_word(str);

        // Check the result and print messages
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



// Function to run the tests
void test_remove_last_substr()
{
    // Define test cases
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
        "one two one three",
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

    for (int i = 0; i < num_tests; i++)
    {
        char str[100];
        strcpy(str, initial_strs[i]);

        // Run the function under test
        int result = remove_last_substr(str, substrs[i]);

        // Print results
        printf("Running test case %d\n", i + 1);
        printf("Initial string: \"%s\"\n", initial_strs[i]);
        printf("Substring: \"%s\"\n", substrs[i]);
        printf("Expected result: \"%s\"\n", expected_results[i]);
        printf("Expected return value: %d\n", expected_returns[i]);
        printf("Function result: %d\n", result);
        printf("Modified string: \"%s\"\n", str);

        // Check if the test passed
        if (result == expected_returns[i] && strcmp(str, expected_results[i]) == 0)
        {
            printf("Test passed.\n\n");
        }
        else
        {
            printf("Test failed.\n\n");
        }
    }
}

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

    for (int i = 0; i < num_tests; i++) {
        char str[50];
        strcpy(str, test_strings[i]);
        strip_chars(str, remove_chars_list[i]);

        if (strcmp(str, expected_results[i]) != 0) {
            printf("strip_chars(\"%s\", \"%s\") = \"%s\"; expected \"%s\"\n", 
                   test_strings[i], remove_chars_list[i], str, expected_results[i]);
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
