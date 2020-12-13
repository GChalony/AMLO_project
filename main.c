#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 1000

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

	int start = time(NULL);

	naive(array, N);

	int end = time(NULL);
	printf("Took %d seconds", end - start);

}
