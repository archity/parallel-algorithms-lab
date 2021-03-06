#include <stdio.h>
#include <omp.h>
#include <stdint.h>
#include <string.h>

#include <x86intrin.h>

#include "sorting.h"


/* 
   Merge two sorted chunks of array T!
   The two chunks are of size size
   First chunck starts at T[0], second chunck starts at T[size]
*/
void merge (uint64_t *T, const uint64_t size)
{
  uint64_t *X = (uint64_t *) malloc (2 * size * sizeof(uint64_t)) ;
  
  uint64_t i = 0 ;
  uint64_t j = size ;
  uint64_t k = 0 ;
  
  while ((i < size) && (j < 2*size))
  {
    if (T[i] < T [j])
	  {
      X [k] = T [i] ;
      i = i + 1 ;
	  }
    else
    {
      X [k] = T [j] ;
      j = j + 1 ;
    }
    k = k + 1 ;
  }

  if (i < size)
  {
    for (; i < size; i++, k++)
    {
      X [k] = T [i] ;
    }
  }
  else
  {
    for (; j < 2*size; j++, k++)
    {
      X [k] = T [j] ;
    }
  }
  
  memcpy (T, X, 2*size*sizeof(uint64_t)) ;
  free (X) ;
  
  return ;
}





/* 
   merge sort -- sequential, parallel -- 
*/

void sequential_merge_sort (uint64_t *T, const uint64_t size)
{
    /* sequential implementation of merge sort */ 

    uint64_t temp;

    // When only 2 elements remain
    if(size==2)
    {
      // Swap elements if required
      if(T[1] < T[0])
      {
        temp = T[1];
        T[1] = T[0];
        T[0] = temp;
        return;
      }
      else
      {
        return;
      }
    }

    // Divide into equal halves
    sequential_merge_sort(T, size/2);
    sequential_merge_sort(T+size/2, size/2);

    // Merge the halves
    merge(T, size/2);


    return ;
}

void parallel_merge_sort (uint64_t *T, const uint64_t size)
{
  /* parallel implementation of merge sort */

  uint64_t temp;

  // When only 2 elements remain
  if(size==2)
  {
    // Swap elements if required
    if(T[1] < T[0])
    {
      temp = T[1];
      T[1] = T[0];
      T[0] = temp;
      return;
    }
    else
    {
      return;
    }
  }

  
  // Divide into equal halves

  #pragma omp task
  parallel_merge_sort(T, size/2);
  #pragma omp task
  parallel_merge_sort(T+size/2, size/2);
      

  // Merge the halves

  #pragma omp taskwait
  merge(T, size/2);

  return;
}

void parallel_merge_sort_v2 (uint64_t *T, const uint64_t size, int threads)
{
  /* Optimized parallel version of merge sort */


  uint64_t temp;

  // When only 2 elements remain
  if(size==2)
  {
    // Swap elements if required
    if(T[1] < T[0])
    {
      temp = T[1];
      T[1] = T[0];
      T[0] = temp;
      return;
    }
    else
    {
      return;
    }
  }

  if(threads == 1)
  {
    sequential_merge_sort(T, size);
  }
  else if(threads >= 2)
  {
    // Divide into equal halves

    #pragma omp task
    parallel_merge_sort_v2(T, size/2,  threads/2);
    #pragma omp task
    parallel_merge_sort_v2(T+size/2, size/2, threads/2);


    // Merge the halves

    #pragma omp taskwait
    merge(T, size/2);
  }


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
        fprintf (stderr, "merge.run N \n") ;
        exit (-1) ;
    }

    uint64_t N = 1 << (atoi(argv[1])) ;
    /* the array to be sorted */
    uint64_t *X = (uint64_t *) malloc (N * sizeof(uint64_t)) ;

    printf(" --> Sorting an array of size %lu (2^%u)\n", N, atoi(argv[1]));
    #ifdef RINIT
      printf("--> The array is initialized randomly\n");
    #endif
    

    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        #ifdef RINIT
            init_array_random (X, N);
        #else
            init_array_sequence (X, N);
        #endif
            
        
        start = _rdtsc () ;
        
        sequential_merge_sort (X, N) ;
        
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
    printf ("\n mergesort serial \t%.2lf Mcycles\n\n", serial_cycles) ;

  
    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
        #ifdef RINIT
                init_array_random (X, N);
        #else
                init_array_sequence (X, N);
        #endif
        
        start = _rdtsc () ;
        #pragma omp parallel
        {
          #pragma omp single
          {
            parallel_merge_sort (X, N) ;
            // parallel_merge_sort_v2 (X, N, omp_get_max_threads()) ;
            // printf("Hemnlo!\n");
          }
        }
        
        
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
            print_array (X, N);
            exit (-1) ;
        }
        #endif
                
    }
    
    av = average_time() ;  
    double parallel_cycles = (double)av/1000000;
    printf (" mergesort parallel \t%.2lf Mcycles\n\n", (double)av/1000000) ;
    
    FILE *f = fopen("speedups.txt", "a+w");

    printf(" Speedup: \t\t%f\n", serial_cycles/parallel_cycles);
    fprintf(f, "%f\n", serial_cycles/parallel_cycles);

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

    sequential_merge_sort (Y, N) ;
    parallel_merge_sort (Z, N) ;
    // parallel_merge_sort_v2 (Z, N, omp_get_max_threads()) ;

    if (! are_vector_equals (Y, Z, N)) {
        fprintf(stderr, "ERROR: sorting with the sequential and the parallel algorithm does not give the same result\n") ;
        exit (-1) ;
    }


    free(X);
    free(Y);
    free(Z);

    printf("================================================\n\n");
    
}
