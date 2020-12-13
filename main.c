#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 1000000

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

int main(){
	// init random
	srand((unsigned int)time(NULL));
	float array[N];

	init(array, N);

	clock_t start = clock();

	float res = naive(array, N);

	clock_t end = clock();

	printf("Array of size %d\nTook %f seconds\nResult = %f\n", N, (end - start)/(double)CLOCKS_PER_SEC, res);

	return 0;
}