#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <immintrin.h>


#define N 10000000

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

float vect_norm(float *U, int n){
    unsigned int i;
    __m256 *mm_U = (__m256 *)U;
    __m256 *res;
    for( i = 0; i <n/8; i++) {
    res[i] = _mm256_sqrt_ps(_mm256_max_ps(mm_U[i], _mm256_sub_ps(_mm256_setzero_ps(),mm_U[i])));
    }
}

int main(){
	// init random
	srand((unsigned int)time(NULL));
	static float array[N];

	init(array, N);

	clock_t start = clock();

	float res = naive(array, N);

	clock_t end = clock();

	printf("Array of size %d\nTook %f seconds\nResult = %f\n", N, (end - start)/(double)CLOCKS_PER_SEC, res);

	return 0;
}