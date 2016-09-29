// Christopher Lucas

#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

char salt[2];
char* target;

bool checker(const char* guess, const char* salt, const char* correct) {
	char* hash = crypt(guess, salt);
	if (hash == NULL) {
		printf("error with crypt\n");
		exit(-1);
	}

	bool isDifferent = strcmp(correct, hash);

	return !isDifferent;
}

void usage() {
	printf("crack <threads> <keysize> <target>\n");
	exit(-1);
}

char* crackHelper(char* guess, const int currIdx, const int length) {
	if (currIdx == length) {
		if (checker(guess, salt, target)) {
			return guess;
		} else {
			return NULL;
		}
	} 

	for (char c = 'a'; c <= 'z'; c++) {
		guess[currIdx] = c;
		char* result = crackHelper(guess, currIdx + 1, length);
		if (result != NULL) {
			return result;
		}
		if (currIdx == 0) {
			printf("%d-%c\n", length, c);
		}
	}
	return NULL;
}

char* crack(const int length) {
	char* guess = (char*) malloc(length + 1);
	guess[length] = '\0';
	return crackHelper(guess, 0, length);
}


int main(int argc, char** argv) {

	if (argc != 4) { usage(); }
	long int threads = strtol(argv[1], NULL, 10);
	long int keysize = strtol(argv[2], NULL, 10);
	target = argv[3];
	strncpy(salt, target, 2);

	for (int length = 1; length <= keysize; length++) {
		char* result = crack(length);
		if (result) {
			printf("%s\n", result);
			break;
		}
	}

	return 0;
}


