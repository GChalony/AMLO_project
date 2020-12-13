#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 1000000

float naive(float *a, int n){
	float res = 0;
	for (int i=0; i<n; i++){
		res += sqrtf(abs(a[i]));
	}
	return res;
}

void main(){
	// init random
	srand((unsigned int)time(NULL));
	float array[N];

	float start = time(NULL);

	naive(array, N);

	float end = time(NULL);
	printf("Took %f seconds\n", end - start);

}
