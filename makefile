all:
	gcc -lcrypt -lpthread -std=c11 -o crack crack.c

clean:
	rm crack
