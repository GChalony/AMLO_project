CC=gcc

run: main
	./main

main: main.c
	$(CC) -o main vectoriel.c -m -msse2-msse3

