#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <immintrin.h>


// On a ajouté une boucle interne pour le calcul afin de rendre 
// le calcul plus long sans avoir a agrandir le tableau (limité par la RAM)
// Le nombre de boucle interne est parametré par cette variable M
#define M 10

#define TRUE 1
#define FALSE 0

/* ========== UTILS ========== */

typedef struct {
	// Args structure
    int mode;
    int N;
    int nb_threads;
} args_struct;

void parse_args(int argc, char *argv[], args_struct *args) {
	// Syntax: $ ./main n_threads N mode
    args->nb_threads = 2;
    args->N = 10000000;
    args->mode = 0;

    if(argc>=2){
        args->nb_threads = atoi(argv[1]);
    }
    else if (argc>=3){
        args->N = atoi(argv[2]);
    }
    else if (argc>=4){
        args->mode = atoi(argv[3]);
    }
}

double now(){
	// Returns current POSIX time
	struct timeval ts;
	gettimeofday(&ts, NULL);
	double sum = ((double)ts.tv_sec) + ts.tv_usec / 1000000.;
	return sum;
}

/* =========================== */

void init(double *a, int n){
	// Fill array with random values between 0 and 1
	for (int i=0; i<n; i++){
		a[i] = ((double)rand()/(double)(RAND_MAX)) / n;
	}
}

double naive(double *a, int n){
	// Naive for loop used as reference
	double res = 0;
	for (int i=0; i<n; i++){
		for (int j=0; j<M; j++){
			res += sqrt(fabs(a[i]));
		}
	}
	return res;
}

double vectAVX(double *U, int n){
	// Vectorized with AVX
    unsigned int i;
    __m256d *mm_U = (__m256d *)U;
    __m256d res = _mm256_setzero_pd();
    __m256d sign_mask = _mm256_set1_pd(-0.0f);

    for( i = 0; i <n/4; i++) {
        for (int j=0; j<M; j++){
            res = _mm256_add_pd(res,_mm256_sqrt_pd(_mm256_andnot_pd(sign_mask, mm_U[i])));
        }
    }

    double *final = (double *) &res;
    for (int j =1; j < 4; j++) {
        final[0] += final[j];
    };
    return final[0];
}

double openmp(double *a, int n){
	// OpenMP implementation with multithreading to illustrate simplicity
	double res = 0;
	#pragma omp parallel for reduction(+:res)
	for (int i=0; i<n; i++){
		for (int j=0; j<M; j++){
			res += sqrt(fabs(a[i]));
		}
	}
	return res;
}


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
	// describeThreadObj(obj);

	if(obj->vect == TRUE) {
	    *(obj->res) = vectAVX(obj->a , obj->n);
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


int main(int argc, char *argv[]){
    // Get arguments
    args_struct args;
    parse_args(argc, argv, &args);

	// Init random
	srand((unsigned int)time(NULL));
	double *array = NULL;
	posix_memalign((void **) &array, 64, args.N * sizeof(double));  // Aligned
	
	init(array, args.N);

	printf("Running naive\n");
	double start = now();
	double res = naive(array, args.N);
	double end_naive = now();

	printf("Running OpenMP\n");
	double start_omp = now();
	double res_omp = openmp(array, args.N);
	double end_omp = now();

	printf("Running Vectorial\n");
	double start_vect = now();
	double res_vect = vectAVX(array, args.N);
	double end_vect = now();

	printf("Running MT\n");
	double start_mt = now();
	double res_mt = multiThreaded(array, args.N, FALSE, args.nb_threads);
	double end_mt = now();

	printf("Running VECT + MT\n");
	double start_vect_mt = now();
	double res_vect_mt = multiThreaded(array, args.N, TRUE, args.nb_threads);
	double end_vect_mt = now();

	printf("Done!\n\n");

	printf("METHOD     \t\tTIME     \tRESULT     \tTIME RATIO\tERROR RATIO\n");
	printf("NAIVE     \t\t%f s\t%f\t%f%%\t%f%%\n",
			end_naive - start, 
			res,
			100.,
			0.
			);
	printf("OPENMP        \t\t%f s\t%f\t%f%%\t%f%%\n",
			end_omp - start_omp,
			res_omp,
			(end_omp - start_omp) / (end_naive - start) * 100,
			(res_omp - res) / res * 100
			);
	printf("VECTORIAL        \t%f s\t%f\t%f%%\t%f%%\n",
			end_vect - start_vect,
			res_vect,
			(end_vect - start_vect) / (end_naive - start) * 100,
			(res_vect - res) / res * 100
			);
	printf("MULTITHREAD   \t\t%f s\t%f\t%f%%\t%f%%\n",
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
