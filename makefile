CC=gcc
nb_thread=4
N=16000000

run: main
	@ echo "Executing O1 version"
	@ ./main $(nb_thread) $(N)
	@ echo ""
	@ echo "Executing O0 version"
	@ ./main_no_opt $(nb_thread) $(N)

main: main.c
	@ $(CC) -Wall -o main main.c -mavx -mavx2 -pthread -O1 -fopenmp -lm
	@ echo "Compilation with -O1 done ! ✅"
	@ $(CC) -Wall -o main_no_opt main.c -mavx -mavx2 -pthread -O0 -fopenmp -lm
	@ echo "Compilation with -O0 done"
	@ echo ""

clean:
	rm main

