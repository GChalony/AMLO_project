#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h> // for timing
#include <immintrin.h>


#define N 16000

double now(){
   // Retourne l'heure actuelle en secondes
   struct timeval t; double f_t;
   gettimeofday(&t, NULL);
   f_t = t.tv_usec; f_t = f_t/1000000.0; f_t +=t.tv_sec;
   return f_t;
}

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
//        res = _mm256_add_ps(res,_mm256_sqrt_ps(_mm256_andnot_ps(sign_bit, mm_U[i])));
        res = _mm256_add_ps(res,_mm256_sqrt_ps(_mm256_andnot_ps(sign_bit, mm_U[i])));
    }
    float *final = (float *) &res;
    for (int j =1; j < 8; j++) {
        final[0] += final[j];
    };
    return final[0];
}


int main(){
	// init random
	srand((unsigned int)time(NULL));
    static float array[N] __attribute__((aligned(64)));
	double t;
	double time_naive;
	double time_vect1;
	double time_vect2;

	init(array, N);

	t = now();
	float res = naive(array, N);
	time_naive = now() - t;
	printf("Array of size %d\nTook %e seconds\nResult = %f\n", N, time_naive, res);

	t = now();
	float res2 = vect_norm1(array, N);
	time_vect1 = now() - t;
	printf("Array of size %d\nTook %e seconds when vect\nResult = %f\n", N, time_vect1, res2);

	printf("Acceleration vect1 = %e \n", time_naive/time_vect1);

	return 0;
}