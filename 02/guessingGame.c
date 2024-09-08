/*
 * guessingGame.v
 *
 * A binary search game that prompts the user to gues a number
 * The computer then guesses the number through a series of hints
 * 		H if the number is higher
 * 		L if the number is lower
 * 		= if the guess is correct
 * 	using the fewest number of attempts of the order log2(Range of Elements in the guessing range)
 *
 * Author: John Bosco Thuo
 *
 */




#include <stdio.h>

#define GUESS_RANGE 100 // preprocessor MACROS defing the guessing range

int main ()
{
	int lower_bound = 1, upper_bound = GUESS_RANGE, median, attemptCount = 0;
	
	char user_response;

	printf("Think of a number between %d and %d,\n"
		"I will try guess the number in as few times as possible\n"
		, upper_bound, lower_bound);

	while (lower_bound <= upper_bound)
	{
		median = (upper_bound + lower_bound) / 2;
		attemptCount++; // increment the number of attempts at every iteration of the loop

		printf("I guess %d \n"
			"If the answer is HIGHER than my guess, enter 'H':\n"
			"If the answer is LOWER than my guess, enter 'L': \n"
			"If my guess is correct, enter '=': \n"
			"response:\n", median);

		scanf(" %c", &user_response);

		// Switch cases for different user responses
		switch (user_response)
		{
			case 'H':
				lower_bound = median + 1;
				// after each adjusting, check if the new lower bound violates the guessing range
				if (lower_bound > GUESS_RANGE) {
					printf("Number is out of upper bound range\n"
						"Program exiting\n");
					return 1; // exits and returns control to program with a code 1
				}
				break;

			case 'L':
				upper_bound = median - 1;
				// after each adjusting, check if new upper bound violates the guessing range
				if (upper_bound < 1) {
					printf("Out of lower bound range\n"
						"Program Exiting\n");
					return 1; // exits and returns control to program with a code 1
				}
				break;
	
			case '=':
				printf("Yay, I guessed %d in %d attempts\n", median, attemptCount);
				return 0;

			default:
				// default case of invalid input is entered
				printf("Please ENTER 'H' if the answer is higher,\n"
					"or ENTER 'L' is the answer is lower, \n"
					"or ENTER '=' if the guess is correct, \n"
					"or press ENTER to quit the game.\n");
				attemptCount--; // Don't count the invalid input
				break;		
		}
		// To check for inconsistent inputs
		if (lower_bound > upper_bound) {
			printf("Inconsistent input, program is exiting now\n");
	 		return 1;
		}
	}
}
