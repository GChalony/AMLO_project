#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define N 100000000


void init(float *a, int n){
	for (int i=0; i<n; i++){
		a[i] = ((float)rand()/(float)(RAND_MAX));
	}
}


float naive(float *a, int n){
	float res = 0;
	for (int i=0; i<n; i++){
		res += sqrtf(fabsf(a[i]));
	}
	return res;
}

typedef struct {
	float *a;
	int n;
	int t;
	float *res;
} ThreadObj;

void instantiateThreadObj(ThreadObj *obj, float *a, int n, int t, float *res){
	obj->a = a;
	obj->n = n;
	obj->t = t;
	obj->res = res;
}

void describeThreadObj(ThreadObj *obj){
	printf("ThreadObj: n=%d, t=%d, res=%f address of a=%d", obj->n, obj->t, *(obj->res), &(obj->a));
}

void *runThread(void *args){
	ThreadObj *obj = (ThreadObj *)args;
	printf("\nIn thread: ");
	describeThreadObj(obj);
	float res = 0;

	for (int i=0; i<obj->n; i++){
		res += sqrtf(fabsf(obj->a[i]));
	}

	printf("Thread %d got res=%f\n", obj->t, res);
	*(obj->res) = res;
	return NULL;
}

float multiThreaded(float *a, int n, int n_threads){
	pthread_t *th_id = malloc(n_threads * sizeof(pthread_t));
	
	float *resultsPerThread = malloc(n_threads * sizeof(float));
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

	float res = 0;
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
	float *array = malloc(N * sizeof(float));
	// float array[N] __attribute__((aligned(64)));

	init(array, N);

	clock_t start = clock();

	float res = naive(array, N);
	
	clock_t end_naive = clock();

	clock_t start_mt = clock();

	float res_mt = multiThreaded(array, N, 4);

	clock_t end_mt = clock();
	printf("Array of size %d\nTook %f seconds\nResult = %f\n", 
			N, 
			(end_naive - start)/(double)CLOCKS_PER_SEC, 
			res 
			);
	printf("With multithreaded took %f seconds\nResult = %f\n",
			(end_mt - start_mt)/(double)CLOCKS_PER_SEC,
			res_mt
			);

	return 0;
}