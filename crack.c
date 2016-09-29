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
	printf("crack <threads> <keysize> <target> [expanded]\n");
	exit(-1);
}

char* crackHelper(char* guess, const int currIdx, const int length, const char start, const char end) {
	if (currIdx == length) {
		if (checker(guess, salt, target)) {
			return guess;
		} else {
			return NULL;
		}
	} 

	for (char c = start; c <= end; c++) {
		guess[currIdx] = c;
		char* result = crackHelper(guess, currIdx + 1, length, start, end);
		if (result != NULL) {
			return result;
		}
		if (currIdx == 0) {
			printf("%d-%c\n", length, c);
		}
	}
	return NULL;
}

char* crack(const int length, const char start, const char end) {
	char* guess = (char*) malloc(length + 1);
	guess[length] = '\0';
	return crackHelper(guess, 0, length, start, end);
}


int main(int argc, char** argv) {

	if (argc < 4 || 5 < argc) { usage(); }
	long int threads = strtol(argv[1], NULL, 10);
	long int keysize = strtol(argv[2], NULL, 10);
	target = argv[3];
	strncpy(salt, target, 2);

	char start = 'a';
	char end = 'z';

	if (argc == 5) {
		long int expanded = strtol(argv[4], NULL, 10);
		if (expanded) {
			start = ' ';
			end = '~';
		}
	}

	//int threadResponsibility[threads];
	//for (int i = 0; i < threads, i++) {
	//	threadResponsibility[i] = keysize/threads;
	//}
	//for (int i = 0; i < keysize % threads; i++) {
	//	threadResponsibility[i] = threadResponsibility[i] + 1;
	//}
	
	for (int length = 1; length <= keysize; length++) {
		char* result = crack(length, start, end);
		if (result) {
			printf("%s\n", result);
			break;
		}
	}

	return 0;
}


