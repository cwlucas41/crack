// Christopher Lucas

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

char salt[2];
char* target;
char start;
char end;

bool checker(const char* guess, const char* salt, const char* correct, struct crypt_data* data) {
	char* hash = crypt_r(guess, salt, data);
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

char* crackHelper(char* guess, const int currIdx, const int length, struct crypt_data* data) {
	if (currIdx == length) {

		bool same = checker(guess, salt, target, data);

		if (same) {
			return guess;
		} else {
			return NULL;
		}
	} 

	for (char c = start; c <= end; c++) {
		guess[currIdx] = c;
		char* result = crackHelper(guess, currIdx + 1, length, data);
		if (result != NULL) {
			return result;
		}
	}
	return NULL;
}

struct crack_args {
	char firstChar;
	int length;
};


void* crack (void* args) {
	struct crack_args* my_args = (struct crack_args*) args;

	struct crypt_data data;
	data.initialized = 0;

	char guess[my_args->length];
	guess[0] = my_args->firstChar;
	char* result = crackHelper(guess, 1, my_args->length, &data);
	if (result != NULL) {
		printf("%s\n", result);
		exit(0);
	}
}

int main(int argc, char** argv) {

	char* temp;

	if (argc < 4 || 5 < argc) { usage(); }

	long int threads = strtol(argv[1], &temp, 10);
	if (*temp != '\0') { 
		printf("Error parsing threads\n");
		exit(-1);
	}

	long int keysize = strtol(argv[2], &temp, 10);
	if (*temp != '\0') { 
		printf("Error parsing keysize\n");
		exit(-1);
	}

	target = argv[3];
	strncpy(salt, target, 2);

	start = 'a';
	end = 'z';

	if (argc == 5) {
		long int expanded = strtol(argv[4], &temp, 10);
		if (*temp != '\0') { 
			printf("Error parsing expanded\n");
			exit(-1);
		}

		if (expanded) {
			start = ' ';
			end = '~';
		}
	}

	for (int length = 1; length <= keysize; length++) {

		char c = start;
		// have to check >= start due to overflow of char type in expanded case
		while (c <= end && c >= start) {

			pthread_t id[threads];
			struct crack_args args[threads];
			bool started[threads];

			for (int i = 0; i < threads; i++ , c++) {
				started[i] = 0;
				if (c <= end && c >= start) {
					args[i].firstChar = c;
					args[i].length = length;

					if (pthread_create(&id[i], NULL, crack, (void*) &args[i])) {
						printf("Error in Create\n");
						exit(-1);
					}

					printf("Starting length %d starting with %c\n", length, c);
					started[i] = 1;
				} 
			}

			// join all started ids
			for (int i = 0; i < threads; i++) {
				if (started[i]) {
					if (pthread_join(id[i], NULL)) {
						printf("Error in Join\n");
						exit(-1);
					}
				}
			}
		}
	}

	return 0;
}
