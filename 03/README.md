to build the code, run
    $gcc -Wall -Werror -o stringTheory stringTheory.c

Main Functions
is_prefix()
    Description: is_prefix checks if one string (prefix) is a prefix of another string (str). A prefix is defined as a substring that occurs at the beginning of the string str.

    Description: Removes all occurrences of specified characters from the input string.
    Parameters:
        char *str: The string from which characters will be removed.
        const char *remove_chars: A string containing characters to remove from str.
    Helper Function:
        bool char_in_remove_chars(char c, const char *remove_chars): Checks if a character c is present in the set of characters specified by remove_chars.

reverse_in_place()

    Description: Reverses the entire input string in place while converting all characters to lowercase.
    Parameters:
        char *str: The string to be reversed and converted to lowercase.
    Helper Functions:
        void reverse_helper(char *start, char *end): Reverses characters in place between two pointers.
        void to_lowercase(char *c): Converts a character to lowercase.
    
reverse_by_word()

    Description: Reverses each word in the input string while keeping the order of words unchanged.
    Parameters:
        char *str: The string in which words will be reversed.
    Helper Function:
        void reverse_helper(char *start, char *end): Reverses characters in place between two pointers.

remove_last_substr()
    Description: Removes the last occurrence of a specified substring from the input string.
    Parameters:
        char *str: The string from which the substring will be removed.
        const char *substr: The substring to be removed.
    Helper Function:
        bool is_prefix(const char *prefix, const char *str): Checks if prefix is a prefix of str.
    
strip_chars()
    Description: Removes all occurrences of specified characters from the input string.
    Parameters:
        char *str: The string from which characters will be removed.
        const char *remove_chars: A string containing characters to remove from str.
    Helper Function:
        bool char_in_remove_chars(char c, const char *remove_chars): Checks if a character c is present in the set of characters specified by remove_chars.

Test Case Functions

test_is_prefix()
    Description: Tests the is_prefix function by comparing the function’s output against expected results for various test cases.
        Parameters: None
    Functionality: Runs multiple tests with different prefixes and strings to check if the is_prefix function behaves as expected.

test_reverse_in_place()
    Description: Tests the reverse_in_place function by verifying that the string is reversed and converted to lowercase correctly.
        Parameters: None
    Functionality: Runs multiple tests with different input strings to ensure the reversal and lowercase conversion are accurate.

test_reverse_by_word()
    Description: Tests the reverse_by_word function to ensure that each word in the string is reversed while maintaining the word order.
        Parameters: None
    Functionality: Executes tests with various input strings to verify that words are correctly reversed.

test_remove_last_substr()
    Description: Tests the remove_last_substr function by checking if the last occurrence of a specified substring is correctly removed from the string.
        Parameters: None
    Functionality: Runs several tests with different initial strings, substrings, and expected results to validate the function's behavior.

test_strip_chars()
    Description: Tests the strip_chars function to verify that specified characters are removed from the input string.
        Parameters: None
    Functionality: Executes tests with various input strings and sets of characters to remove, comparing the function’s output to expected results.
test function helper Function

bool strings_equal(const char *a, const char *b)
    Description: Compares two strings for equality.
    Parameters:
        const char *a: The first string to compare.
        const char *b: The second string to compare.
    Return Value: Returns true if the strings are equal, false otherwise.

