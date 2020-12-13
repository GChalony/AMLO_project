CC=gcc

run: main
	./main

main: main.c
	$(CC) -Wall -mavx -mavx2 -o main main.c -lm

