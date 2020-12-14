CC=gcc

run: main
	./main

main: main.c
	$(CC) -Wall -mavx -mavx2 -pthread -o main main.c

