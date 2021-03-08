#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include <x86intrin.h>

/* default size */
#define DEF_SIZE    1024

/* Processor frequency in GHZ */
#define PROC_FREQ   1.6


#define NBEXPERIMENTS    22
static long long unsigned int experiments [NBEXPERIMENTS] ;

typedef double *vector;

typedef double *matrix;


long long unsigned int average (long long unsigned int *exps)
{
    unsigned int i ;
    long long unsigned int s = 0 ;

    for (i = 2; i < (NBEXPERIMENTS-2); i++)
    {
        s = s + exps [i] ;
    }

    return s / (NBEXPERIMENTS-2) ;
}


void init_vector (vector *X, const size_t size, const double val)
{
    unsigned int i=0;
    
    *X = malloc(sizeof(double) * size);

    if (*X == NULL){
        perror("vector allocation");
        exit(-1);
    }
    

    for (i = 0 ; i < size; i++)
        (*X)[i] = val ;

    return ;
}

void free_vector (vector X)
{
    free(X);
    
    return ;
}


void init_matrix (matrix *X, const size_t size, const double val)
{
    unsigned int i=0, j=0;
    *X = malloc(sizeof(double) * size * size);

    if (*X == NULL){
        perror("matrix allocation");
        exit(-1);
    }

  
    for (i = 0; i < size; i++)
    {
        for (j = 0 ;j < size; j++)
	{
            (*X)[i*size + j] = val ;
	}
    }
}

void free_matrix (matrix X)
{
    free(X);
    
    return ;
}


  
void print_vectors (vector X, vector Y, const size_t size)
{
    unsigned int i ;

    for (i = 0 ; i < size; i++)
        printf (" X [%d] = %le Y [%d] = %le\n", i, X[i], i,Y [i]) ;

    return ;
}

void add_vectors1 (vector X, vector Y, vector Z, const size_t size)
{
    register unsigned int i ;

#pragma omp parallel for schedule(static)  
    for (i=0; i < size; i++)
        X[i] = Y[i] + Z[i];
  
    return ;
}

void add_vectors2 (vector X, vector Y, vector Z, const size_t size)
{
    register unsigned int i ;

#pragma omp parallel for schedule(dynamic)  
    for (i=0; i < size; i++)
        X[i] = Y[i] + Z[i];
  
    return ;
}

double dot1 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;

  
    dot = 0.0 ;
#pragma omp parallel for schedule(static) reduction (+:dot)
    for (i=0; i < size; i++)
        dot += X [i] * Y [i];

    return dot ;
}

double dot2 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;


    dot = 0.0 ;
#pragma omp parallel for schedule(dynamic) reduction (+:dot)
    for (i=0; i < size; i++)
        dot += X [i] * Y [i];

    return dot ;
}

double dot3 (vector X, vector Y, const size_t size)
{
    register unsigned int i ;
    register double dot ;

    dot = 0.0 ;
#pragma omp parallel for schedule(static) reduction (+:dot)
    for (i = 0; i < size; i = i + 8)
    {
        dot += X [i] * Y [i];
        dot += X [i + 1] * Y [i + 1];
        dot += X [i + 2] * Y [i + 2];
        dot += X [i + 3] * Y [i + 3];
    
        dot += X [i + 4] * Y [i + 4];
        dot += X [i + 5] * Y [i + 5];
        dot += X [i + 6] * Y [i + 6];
        dot += X [i + 7] * Y [i + 7];
    }

    return dot ;
}

void mult_mat_vect0 (matrix M, vector b, double *c, size_t datasize)
{
    /*
      matrix vector multiplication
      Sequential function
    */
    for(int i = 0; i < datasize; i++)
    {
        for(int j = 0; j < datasize; j++)
        {
            c[i] = c[i] + M[i * datasize + j] * b[j];
        }
    }

    return ;
}

void mult_mat_vect1 (matrix M, vector b, vector c, size_t datasize)
{
    /*
      matrix vector multiplication
      Parallel function with static loop scheduling
    */
    for(int i = 0; i < datasize; i++)
    {
        double total = 0.0;
        #pragma omp parallel for schedule(static) reduction (+:total)
        for(int j = 0; j < datasize; j++)
        {
            total = total + M[i * datasize + j] * b[j];
        }
        c[i] = total;
    }

    return ;
}

void mult_mat_vect2 (matrix M, vector b, vector c, size_t datasize)
{
    /*
      matrix vector multiplication
      Parallel function with static loop scheduling
      unrolling internal loop
    */
    for (int i = 0; i < datasize; ++i)
    {
        double total = 0.0;
#pragma omp parallel for schedule(static) reduction (+:total)
        for(int j = 0; j < datasize; j=j+8)
        {
            total = total + M[i * datasize + j + 0] * b[j + 0];
            total = total + M[i * datasize + j + 1] * b[j + 1];
            total = total + M[i * datasize + j + 2] * b[j + 2];
            total = total + M[i * datasize + j + 3] * b[j + 3];
            total = total + M[i * datasize + j + 4] * b[j + 4];
            total = total + M[i * datasize + j + 5] * b[j + 5];
            total = total + M[i * datasize + j + 6] * b[j + 6];
            total = total + M[i * datasize + j + 7] * b[j + 7];
        }
        c[i] = total;
    }

    return ;
}

void mult_mat_mat0 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Sequential function 
    */
    for (int i = 0; i < datasize; i++)
    {
        for (int j = 0; j < datasize; j++)
        {
            for (int k = 0; k < datasize; k++)
            {
                C[i * datasize + j] += A[i * datasize + k] * B[k * datasize + j];
            }
        }
    }

    return;
}


void mult_mat_mat1 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Parallel function with OpenMP and static scheduling 
    */
    for (int i = 0; i < datasize; i++)
    {
        for (int j = 0; j < datasize; j++)
        {
            double total = 0.0;
#pragma omp parallel for schedule(static) reduction (+:total)
            for (int k = 0; k < datasize; k++)
            {
                total += A[i * datasize + k] * B[k * datasize + j];
            }
            C[i * datasize + j] = total;
        }
    }

    return ;    
}

void mult_mat_mat2 (matrix A, matrix B, matrix C, size_t datasize)
{
    /*
      Matrix Matrix Multiplication
      Parallel function with OpenMP and static scheduling 
      Unrolling the inner loop
    */

    for (int i = 0; i < datasize; i++)
    {
        for (int j = 0; j < datasize; j++)
        {
            double total = 0.0;
#pragma omp parallel for schedule(static) reduction (+:total)
            for (int k = 0; k < datasize; k+=8)
            {
                total += A[i * datasize + k+0] * B[k+0 * datasize + j];
                total += A[i * datasize + k+1] * B[k+1 * datasize + j];
                total += A[i * datasize + k+2] * B[k+2 * datasize + j];
                total += A[i * datasize + k+3] * B[k+3 * datasize + j];
                total += A[i * datasize + k+4] * B[k+4 * datasize + j];
                total += A[i * datasize + k+5] * B[k+5 * datasize + j];
                total += A[i * datasize + k+6] * B[k+6 * datasize + j];
                total += A[i * datasize + k+7] * B[k+7 * datasize + j];
            }
            C[i * datasize + j] = total;
        }
    }

    return ;
}



int main (int argc, char *argv[])  
{
    int maxnthreads ;

    vector a, b, c ;
    matrix M1, M2 ;

    size_t datasize = DEF_SIZE;

    unsigned long long int start, end ;
    unsigned long long int residu ;

    unsigned long long int av ;
  
    double r ;

    int exp ;

    if(argc > 2){
        printf("usage: %s [data_size]\n", argv[0]);
        exit(-1);
    }
    else{
        if(argc == 2){
            datasize = atoi(argv[1]);
        }
    }
  
    printf("Testing with Vectors of size %zu -- Matrices of size %zu X %zu\n\n", datasize, datasize, datasize);
  
    /* 
       rdtsc: read the cycle counter 
    */
  
    start = _rdtsc () ;
    end = _rdtsc () ;
    residu = end - start ;
  
    maxnthreads = omp_get_max_threads () ;
    printf("Max number of threads: %d \n", maxnthreads);
	
    /*
      Vector Initialization
    */

    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&c, datasize, 0.0) ;

    /*
      print_vectors (a, b, datasize) ;
    */    

    printf ("=============== ADD ==========================================\n") ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        add_vectors1 (c, a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static loop:\t\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    // Where total FP ops would be (datatsize * 1), coz we have only 1 operation (+)
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (DEF_SIZE * 1 * PROC_FREQ) / (av - residu));

    free_vector(a);
    free_vector(b);
    free_vector(c);
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&c, datasize, 0.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        add_vectors2 (c, a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP dynamic loop:\t\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    // Where total FP ops would be (datatsize * 1), coz we have only 1 operation (+)
    printf ("GFLOPS = %3.3f GFLOP per second\n", (DEF_SIZE * 1 * PROC_FREQ) / (av - residu));

    free_vector(a);
    free_vector(b);
    free_vector(c);
  
    printf ("==============================================================\n\n") ;

    printf ("==================== DOT =====================================\n") ;

    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        r = dot1 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("dot OpenMP static loop:\t\t\t %Ld cycles\n", av-residu) ;
    
    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    // Where total FP ops would be (datatsize * 2), coz we have 2 operations (+ & *)
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (DEF_SIZE * 2 * PROC_FREQ) / (av - residu));

    free_vector(a);
    free_vector(b);
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        r = dot2 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("dot OpenMP dynamic loop:\t\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    // Where total FP ops would be (datatsize * 2), coz we have 2 operations (+ & *)
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (DEF_SIZE * 2 * PROC_FREQ) / (av - residu));

    free_vector(a);
    free_vector(b);  
  
    init_vector (&a, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        r = dot3 (a, b, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("dot OpenMP static unrolled loop:\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    // Where total FP ops would be (datatsize * 2), coz we have 2 operations (+ & *)
    printf ("GFLOPS = %3.3f GFLOP per second\n", (DEF_SIZE * 2 * PROC_FREQ) / (av - residu));

    free_vector(a);
    free_vector(b);
  
    printf ("=============================================================\n\n") ;

    printf ("======================== Mult Mat Vector =====================================\n") ;
  
    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 0.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect0 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
  
    printf ("Sequential matrice vector multiplication:\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops = (DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops * PROC_FREQ) / (av - residu));

    free_matrix(M1);
    free_vector(a);
    free_vector(b);


    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 0.0) ;


    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect1 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
 
    printf ("OpenMP static loop MultMatVect1:\t\t %Ld cycles\n", av-residu) ;
    
    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops_mmv1 = (DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops_mmv1 * PROC_FREQ) / (av - residu));
    

    free_matrix(M1);
    free_vector(a);
    free_vector(b);
  
    init_matrix (&M1, datasize, 1.0) ;
    init_vector (&b, datasize, 2.0) ;
    init_vector (&a, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_vect2 (M1, b, a, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static unrolled loop MultMatVect2:\t\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops_mmv2 = (DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops_mmv2 * PROC_FREQ) / (av - residu));

    free_matrix(M1);
    free_vector(a);
    free_vector(b);
  

    printf ("===================================================================\n\n") ;

    printf ("======================== Mult Mat Mat =====================================\n") ;
  
    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat0 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("Sequential Matrix Matrix Multiplication:\t %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops_mmm0 = (DEF_SIZE * DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops_mmm0 * PROC_FREQ) / (av - residu));

    free_matrix(M1);
    free_matrix(M2);
  
    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;


    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat1 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;

    printf ("OpenMP static loop matrix matrix multiplication: %Ld cycles\n", av-residu) ;
    
    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops_mmm1 = (DEF_SIZE * DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops_mmm1 * PROC_FREQ) / (av - residu));

    free_matrix(M1);
    free_matrix(M2);

    init_matrix (&M1, datasize, 1.0) ;
    init_matrix (&M2, datasize, 2.0) ;

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        start = _rdtsc () ;

        mult_mat_mat2 (M1, M2, M2, datasize) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;
    }

    av = average (experiments) ;
    

    printf ("OpenMP static unrolled loop matrix matrix multiplication: %Ld cycles\n", av-residu) ;

    /* code to print the GLOP per seconds */
    // GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu) 
    int total_ops_mmm2 = (DEF_SIZE * DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE * DEF_SIZE);
    printf ("GFLOPS = %3.3f GFLOP per second\n\n", (total_ops_mmm2 * PROC_FREQ) / (av - residu));
  
    free_matrix(M1);
    free_matrix(M2);

    printf ("===================================================================\n\n") ;
  
    return 0;
  
}

