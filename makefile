CC=gcc

run: main
	./main

main: main.c
	$(CC) -Wall -o main main.c -lm -lpthread
	echo "Compiled!"

