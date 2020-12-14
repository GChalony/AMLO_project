#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <omp.h>

int sched_getcpu(void);

#define N 100000000
#define M 10
#include <immintrin.h>


double now(){
	struct timeval ts;
	gettimeofday(&ts, NULL);
	double sum = ((double)ts.tv_sec) + ts.tv_usec / 1000000.;
	return sum;
}

void init(double *a, int n){
	// Fill array with random values between 0 and 1
	for (int i=0; i<n; i++){
		a[i] = ((double)rand()/(double)(RAND_MAX)) / n;
	}
}

double naive(double *a, int n){
	// Naive for loop
	double res = 0;
	for (int i=0; i<n; i++){
		for (int j=0; j<M; j++){
			res += sqrt(fabs(a[i]));
		}
	}
	return res;
}

double vect_norm1(double *U, int n){
    unsigned int i;
    __m256d *mm_U = (__m256d *)U;
    __m256d res = _mm256_setzero_pd() ;
    __m256d sign_bit = _mm256_set1_pd(-0.0f);
    for( i = 0; i <n/4; i++) {
        for (int j=0; j<M; j++){
            res = _mm256_add_pd(res,_mm256_sqrt_pd(_mm256_andnot_pd(sign_bit, mm_U[i])));
        }
    }
    double *final = (double *) &res;
    for (int j =1; j < 4; j++) {
        final[0] += final[j];
    };
    return final[0];
}

//double openmp(double *a, int n){
//	double res = 0;
//	#pragma omp parallel for reduction(+:res)
//	for (int i=0; i<n; i++){
//		for (int j=0; j<M; j++){
//			res += sqrtf(fabs(a[i]));
//		}
//	}
//	return res;
//}


/*    PThread implementation    */
typedef struct {
    int vect;       // Vectorization enabled or not
	double *a;  	// Array to compute operation on
	int n;  		// Number of items in array
	int t;  		// Thread ID
	double *res;	// Location to store result
} ThreadObj;

void instantiateThreadObj(ThreadObj *obj, int vect,  double *a, int n, int t, double *res){
	// Utils function to fill ThreadObj attributes
	obj->vect = vect;
	obj->a = a;
	obj->n = n;
	obj->t = t;
	obj->res = res;
}

void describeThreadObj(ThreadObj *obj){
    printf("ThreadObj: n=%d, t=%d, res=%f address of a=%p\n", obj->n, obj->t, *(obj->res), &(obj->a));
}

void *runThread(void *args){
	// Run operation on data provided via ThreadObj argument
	
	ThreadObj *obj = (ThreadObj *)args;
//	printf("Thread running on core %d\n", sched_getcpu());
	// describeThreadObj(obj);
	if(obj->vect==1) {
	    *(obj->res) = vect_norm1(obj->a , obj->n);
	} else {
	    *(obj->res) = naive(obj->a , obj->n);
	};

	return NULL;
}

double multiThreaded(double *a, int n, int vect, int n_threads){
	pthread_t *th_id = malloc(n_threads * sizeof(pthread_t));
	
	double *resultsPerThread = malloc(n_threads * sizeof(double));
	int offset=0, m = n / n_threads;  // assumes n multiple of n_threads
	
	for (int t=0; t<n_threads; t++){
		offset = t * m;

		ThreadObj *obj = malloc(sizeof(ThreadObj));
		instantiateThreadObj(obj, vect, a + offset, m, t, &resultsPerThread[t]);
		
		pthread_create(&th_id[t], NULL, runThread, obj);
	}


	double res = 0;
	for (int i=0; i<n_threads; i++){
		pthread_join(th_id[i], NULL);
		res += resultsPerThread[i];
	}

	return res;
}


int main(){
	// init random
	srand((unsigned int)time(NULL));
	double *array = NULL;
	posix_memalign((void **) &array, 64, N * sizeof(double));  // Aligned
	
	init(array, N);

	printf("Running naive\n");
	double start = now();
	double res = naive(array, N);
	double end_naive = now();

//	printf("Running OpenMP\n");
//	double start_omp = now();
//	double res_omp = openmp(array, N);
//	double end_omp = now();

	printf("Running Vectorial\n");
	double start_vect = now();
	double res_vect = vect_norm1(array, N);
	double end_vect = now();

	printf("Running MT\n");
	double start_mt = now();
	double res_mt = multiThreaded(array, N, 0, 4);
	double end_mt = now();

	printf("Running VECT + MT\n");
	double start_vect_mt = now();
	double res_vect_mt = multiThreaded(array, N, 1, 4);
	double end_vect_mt = now();

	printf("Done!\n\n");

	printf("METHOD     \t\tTIME     \tRESULT     \tTIME RATIO\tERROR RATIO\n");
	printf("NAIVE     \t\t%f s\t%f\t%f%%\t%f%%\n",
			end_naive - start, 
			res,
			100.,
			0.
			);
//	printf("OPENMP     \t%f s\t%f\t%f%%\t%f%%\n",
//			end_omp - start_omp,
//			res_omp,
//			(end_omp - start_omp) / (end_naive - start) * 100,
//			(res_omp - res) / res * 100
//			);
	printf("VECTORIAL     \t\t%f s\t%f\t%f%%\t%f%%\n",
			end_vect - start_vect,
			res_vect,
			(end_vect - start_vect) / (end_naive - start) * 100,
			(res_vect - res) / res * 100
			);
	printf("MULTITHREAD\t\t%f s\t%f\t%f%%\t%f%%\n",
			end_mt - start_mt,
			res_mt,
			(end_mt - start_mt) / (end_naive - start) * 100,
			(res_mt - res) / res * 100
			);
	printf("VECT + MULTITHREAD\t%f s\t%f\t%f%%\t%f%%\n",
			end_vect_mt - start_vect_mt,
			res_vect_mt,
			(end_vect_mt - start_vect_mt) / (end_naive - start) * 100,
			(res_vect_mt - res) / res * 100
			);

	return 0;
}