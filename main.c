#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <immintrin.h>


#define N 16000

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
    __m256 res = _mm256_setzero_ps() ;
    for( i = 0; i <n/8; i++) {
        res = _mm256_add_ps(res,_mm256_sqrt_ps(_mm256_max_ps(mm_U[i], _mm256_sub_ps(_mm256_setzero_ps(),mm_U[i]))));
    }
    float resultat = 0;
    float *final = (float *) &res;
    for (int j =0; j < 8; j++) {
        resultat += final[j];
    };
    return resultat;
}

int main(){
	// init random
	srand((unsigned int)time(NULL));
	float array[N] __attribute__((aligned(64)));


	init(array, N);

	clock_t start = clock();

	float res = naive(array, N);

	clock_t end = clock();

	float time_naive =  (end - start)/(double)CLOCKS_PER_SEC;
	printf("Array of size %d\nTook %f seconds\nResult = %f\n", N, time_naive, res);

	start = clock();

	float res2 = vect_norm(array, N);

	end = clock();

	float time_vect =  (end - start)/(double)CLOCKS_PER_SEC;

	printf("Array of size %d\nTook %f seconds when vect\nResult = %f\n", N, time_vect, res2);

	printf("Acceleration = %f \n", time_naive/time_vect);

	return 0;
}