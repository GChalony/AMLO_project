CC=gcc

run: main
	./main

main: main.c
	$(CC) -Wall -lm -o main main.c -avx -avx2

