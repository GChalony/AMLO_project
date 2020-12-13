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


float vect_norm1(float *U, int n){
    unsigned int i;
    __m256 *mm_U = (__m256 *)U;
    __m256 res = _mm256_setzero_ps() ;
    __m256 sign_bit = _mm256_set1_ps(-0.0f);
    for( i = 0; i <n/8; i++) {
        res = _mm256_add_ps(res,_mm256_sqrt_ps(_mm256_andnot_ps(sign_bit, mm_U[i])));
        res = _mm256_add_ps(res,_mm256_sqrt_ps(mm_U[i]));
    }
    float resultat = 0;
    float *final = (float *) &res;
    for (int j =0; j < 8; j++) {
        resultat += final[j];
    };
    return resultat;
}

float vect_norm2(float *U, int n){
    unsigned int i;
    __m256 *mm_U = (__m256 *)U;
    __m256 res = _mm256_setzero_ps() ;
    __m256 sign_bit = _mm256_set1_ps(-0.0f);
    for( i = 0; i <n/8; i++) {
        res = _mm256_add_ps(res,_mm256_sqrt_ps(_mm256_andnot_ps(sign_bit, mm_U[i])));
        res = _mm256_add_ps(res,_mm256_sqrt_ps(mm_U[i]));
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
//	static float array[N] __attribute__((aligned(64)));
	static float array[N];



	init(array, N);

	clock_t start = clock();
	float res = naive(array, N);
	clock_t end = clock();

	float time_naive =  (end - start)/(double)CLOCKS_PER_SEC;
	printf("Array of size %d\nTook %f seconds\nResult = %f\n", N, time_naive, res);

	start = clock();

	float res2 = vect_norm1(array, N);
	end = clock();
	float time_vect1 =  (end - start)/(double)CLOCKS_PER_SEC;
	printf("Array of size %d\nTook %f seconds when vect\nResult = %f\n", N, time_vect1, res2);

	printf("Acceleration vect1 = %f \n", time_naive/time_vect1);

	return 0;
}