#include <stdio.h>

#define GUESS_RANGE 100

int main ()
{
	int lower_bound = 1;
	int upper_bound = GUESS_RANGE;

	int median;
	int attemptCount = 0;
	
	char user_response;

	printf("Think of a number between %d and %d,\n"
		"I will try guess the number in as few times as possible\n"
		, upper_bound, lower_bound);

	while (lower_bound <= upper_bound)
	{
		median = (upper_bound + lower_bound) / 2;
		attemptCount++;

		printf("I guess %d \n"
			"If the answer is HIGHER than my guess, enter 'H':\n"
			"If the answer is LOWER than my guess, enter 'L': \n"
			"If my guess is correct, enter '=': \n"
			"response:", median);

		scanf(" %c", &user_response);


		switch (user_response) {
			case 'H':
				lower_bound = median + 1;
				break;

			case 'L':
				upper_bound = median - 1;
				break;
	
			case '=':
				printf("Yay, I guessed %d in %d attempts", median, attemptCount);
				return 0;
			default:
				printf("Please ENTER 'H' if the answer is higher,\n"
					"or ENTER 'L' is the answer is lower, \n"
					"or ENTER '=' if the guess is correct, \n"
					"or press ENTER to quit the game.");
			}
	}
}	
