all:
	gcc -lcrypt -std=c11 -o crack crack.c

clean:
	rm crack
