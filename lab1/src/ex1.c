#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void Hello(char *st)
{

    int my_rank, nthreads, nmaxthreads;

    /*
      to get the max number of threads
    */

    nmaxthreads = omp_get_max_threads () ;

    /*
      to get the current number of threads
    */
    nthreads = omp_get_num_threads();

    /*
      to get my rank
    */
    my_rank = omp_get_thread_num();
	
    printf("%s thread %d of team %d (max_num_threads is %d)\n", st, my_rank, nthreads,nmaxthreads);
	
} 


int main (int argc, char*argv[])  
{
    /* 
       Program starts here with the master thread
    */

    int nthreads;
	
    /*
      Getting the thread number (program parameter)
    */
  
    if (argc != 2)
    {
        fprintf (stderr, "the parameter (thread number) is missing!\n") ;
        fprintf (stderr, "exo1 thread_number\n") ;
        exit (-1) ;
    }
  
    nthreads = atoi (argv[1]);
  
    printf("I am the master thread %d and I start\n", omp_get_thread_num ());
	
    /* 
       This is a parallel region
       All threads will execute this region
    */
    printf ("Starting Region 1 \n") ;
#pragma omp parallel num_threads (nthreads)
    Hello("Region 1 ") ; 

    printf ("End of Region 1\n") ;

  
    printf ("Starting Region 2 \n") ;

  
    /* 
       This is a parallel region
       half of threads will execute this region
    */
#pragma omp parallel num_threads (nthreads/2) 
    Hello ("Region 2 ") ;
    /*
      End of the parallel region
      The master thread is alone in this serial region
    */
    printf ("End of Region 2\n") ;


    /* 
       This is a parallel region
       quarter of threads will execute this region
    */
    printf ("Starting Region 3 \n") ;

  
#pragma omp parallel num_threads (nthreads/4) 
    Hello ("Region 3 ") ;

  
    /*
      End of the parallel region
      The master thread is alone in this serial region
    */
    printf ("End of Region 3\n") ;
  
    printf("I am the master thread %d and I complete\n", omp_get_thread_num ());
	
    return 0;
} 


