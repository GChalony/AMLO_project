#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

#define N 100000000


void init(double *a, int n){
	for (int i=0; i<n; i++){
		a[i] = ((double)rand()/(double)(RAND_MAX)) / n;
	}
}


double naive(double *a, int n){
	double res = 0;
	for (int i=0; i<n; i++){
		res += sqrtf(fabsf(a[i]));
	}
	return res;
}

typedef struct {
	double *a;
	int n;
	int t;
	double *res;
} ThreadObj;

void instantiateThreadObj(ThreadObj *obj, double *a, int n, int t, double *res){
	obj->a = a;
	obj->n = n;
	obj->t = t;
	obj->res = res;
}

void describeThreadObj(ThreadObj *obj){
	printf("ThreadObj: n=%d, t=%d, res=%f address of a=%d\n", obj->n, obj->t, *(obj->res), &(obj->a));
}

void *runThread(void *args){
	ThreadObj *obj = (ThreadObj *)args;
	printf("\nIn thread on core %d: ", sched_getcpu());
	describeThreadObj(obj);
	double res = 0;

	for (int i=0; i<obj->n; i++){
		// if (i % 100000 == 0){
		// 	printf("\tFrom thread %d at iteration %d\n", obj->t, i);
		// }
		res += sqrtf(fabsf(obj->a[i]));
	}

	printf("Thread %d got res=%f\n", obj->t, res);
	*(obj->res) = res;
	return NULL;
}

double multiThreaded(double *a, int n, int n_threads){
	pthread_t *th_id = malloc(n_threads * sizeof(pthread_t));
	
	double *resultsPerThread = malloc(n_threads * sizeof(double));
	int offset=0, m = n / n_threads;  // assumes n multiple of n_threads
	
	for (int t=0; t<n_threads; t++){
		offset = t * m;

		ThreadObj *obj = malloc(sizeof(ThreadObj));
		instantiateThreadObj(obj, a + offset, m, t, &resultsPerThread[t]);
		
		printf("\nCreated thread: ");
		describeThreadObj(obj);
		pthread_create(&th_id[t], NULL, *runThread, obj);
	}
	
	printf("\n");

	double res = 0;
	for (int i=0; i<n_threads; i++){
		pthread_join(th_id[i], NULL);
		res += resultsPerThread[i];
		printf("Thread %d computed %f\n", i, resultsPerThread[i]);
	}

	return res;
}

int main(){
	// init random
	srand((unsigned int)time(NULL));
	double *array = malloc(N * sizeof(double));
	// double array[N] __attribute__((aligned(64)));

	init(array, N);

	double start = clock();

	double res = naive(array, N);
	
	double end_naive = clock();

	double start_mt = clock();

	double res_mt = multiThreaded(array, N, 4);

	double end_mt = clock();
	printf("NAIVE : array of size %d\nTook %f seconds\nResult = %f\n", 
			N, 
			(end_naive - start)/(double)CLOCKS_PER_SEC, 
			res 
			);
	printf("MULTITHREAD : With multithreaded took %f seconds\nResult = %f\n",
			(end_mt - start_mt)/(double)CLOCKS_PER_SEC,
			res_mt
			);


	printf("Comparison:\n Error of %f%%\n Time ratio %f%%\n", (res - res_mt) / res * 100, (end_mt - start_mt) / (end_naive - start) * 100);
	return 0;
}