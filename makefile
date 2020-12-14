CC=gcc
nb_thread=4
N=16000000

run: main
	@ ./main $(nb_thread) $(N)

main: main.c
	@ $(CC) -Wall -o main main.c -mavx -mavx2 -pthread -O1 -fopenmp -lm
	@ echo "Compilation done ! ✅"

clean:
	rm main

