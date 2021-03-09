#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>

#include <x86intrin.h>

#include "sorting.h"

/* 
   bubble sort -- sequential, parallel -- 
*/


void sequential_bubble_sort (uint64_t *T, const uint64_t size)
{
    /* sequential implementation of bubble sort */ 

    uint64_t i, temp;
    uint64_t sorted;

    do
    {
        sorted = 1;
        for(i = 0; i < size - 1; i++)
        {
            if (T[i] > T[i+1])
            {
                temp = T[i+1];
                T[i+1] = T[i];
                T[i] = temp;

                sorted = 0;
            }
        }
        
    } while (sorted == 0);
    
    return ;
}

void parallel_bubble_sort (uint64_t *T, const uint64_t size)
{
    /* parallel implementation of bubble sort */

    uint64_t temp, sorted, i;
    uint64_t ch_sz;
    
    ch_sz = size / omp_get_max_threads();  
    do
    {
        sorted = 1;
        #pragma omp parallel for schedule(static)
        for (i=0; i<size; i+=ch_sz)
        {
            sequential_bubble_sort(T+i, ch_sz);
        }    
        #pragma omp parallel for schedule(static), private(temp)
        for (i=ch_sz;i<size;i+=ch_sz)
        {
            if (T[i] < T[i-1])
            {
                temp = T[i-1];
                T[i-1] = T[i];
                T[i] =  temp;
                sorted = 0;
            }
        }   
        
    }while (sorted == 0);

    return;
}


int main (int argc, char **argv)
{
    uint64_t start, end;
    uint64_t av ;
    unsigned int exp ;

    printf("================================================\n");
    printf(" Max number of threads: %d \n", omp_get_max_threads());

    /* the program takes one parameter N which is the size of the array to
       be sorted. The array will have size 2^N */
    if (argc != 2)
    {
        fprintf (stderr, "bubble.run N \n") ;
        exit (-1) ;
    }

    uint64_t N = 1 << (atoi(argv[1])) ;
    /* the array to be sorted */
    uint64_t *X = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

    printf(" --> Sorting an array of size %lu\n",N);
#ifdef RINIT
    printf("--> The array is initialized randomly\n");
#endif
    

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++){
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
        
      
        start = _rdtsc () ;
        
        sequential_bubble_sort (X, N) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;

        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            print_array (X, N) ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            fprintf(stderr, "ERROR: the sequential sorting of the array failed\n") ;
            print_array (X, N) ;
            exit (-1) ;
	}
#endif
    }

    av = average_time() ;  

    double serial_cycles = (double)av/1000000;
    printf ("\n bubble serial \t\t%.2lf Mcycles\n\n", serial_cycles) ;

  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
#ifdef RINIT
        init_array_random (X, N);
#else
        init_array_sequence (X, N);
#endif
        
        start = _rdtsc () ;

        parallel_bubble_sort (X, N) ;
     
        end = _rdtsc () ;
        experiments [exp] = end - start ;

        /* verifying that X is properly sorted */
#ifdef RINIT
        if (! is_sorted (X, N))
        {
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            exit (-1) ;
	}
#else
        if (! is_sorted_sequence (X, N))
        {
            fprintf(stderr, "ERROR: the parallel sorting of the array failed\n") ;
            print_array (X, N) ;
            exit (-1) ;
	}
#endif
                
        
    }
    
    av = average_time() ;  
    double parallel_cycles = (double)av/1000000;
    printf (" bubble parallel \t%.2lf Mcycles\n\n", parallel_cycles) ;
  
    printf(" Speedup: \t\t%f\n", serial_cycles/parallel_cycles);
    /* print_array (X, N) ; */

    /* before terminating, we run one extra test of the algorithm */
    uint64_t *Y = (uint64_t *) malloc (N * sizeof(uint64_t)) ;
    uint64_t *Z = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

#ifdef RINIT
    init_array_random (Y, N);
#else
    init_array_sequence (Y, N);
#endif

    memcpy(Z, Y, N * sizeof(uint64_t));

    sequential_bubble_sort (Y, N) ;
    parallel_bubble_sort (Z, N) ;

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);

    printf("================================================\n\n");
    
}