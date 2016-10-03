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

	if (argc < 4 || 5 < argc) { usage(); }
	long int threads = strtol(argv[1], NULL, 10);
	long int keysize = strtol(argv[2], NULL, 10);
	target = argv[3];
	strncpy(salt, target, 2);

	start = 'a';
	end = 'z';

	if (argc == 5) {
		long int expanded = strtol(argv[4], NULL, 10);
		if (expanded) {
			start = ' ';
			end = '~';
		}
	}

	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	for (int length = 1; length <= keysize; length++) {

		char c = start;
		while (c <= end) {

			pthread_t id[threads];
			struct crack_args args[threads];

			for (int i = 0; i < threads; i++ , c++) {
				if (c <= end) {
					args[i].firstChar = c;
					args[i].length = length;

					if (pthread_create(&id[i], NULL, crack, (void*) &args[i])) {
						printf("Error in Create\n");
						exit(-1);
					}
					printf("Starting: %d %c %d\n", length, c, id[i]);
				} else {
					id[i] = -1;
				}
			}

			

			for (int i = 0; i < threads; i++) {
				if (id[i] != -1) {
					printf("Joining: %d\n", id[i]);
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
