#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char** argv){

    int rank, size;
    char message[50];
    MPI_Request request;
    MPI_Status status;

    /* Intializes random number generator */
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // P0 and P1 both acting as senders and receivers

    if(rank == 0)
    {
        strcpy(message, "Hello from P0!");
        
        MPI_Isend(message, strlen(message), MPI_INT, rank+1, 7, MPI_COMM_WORLD, &request);
        MPI_Irecv(message, strlen(message), MPI_INT, rank+1, 7, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        printf("\nR0 Received: %s\n\n", message);
    }
    
    else if (rank == 1)
    {
        strcpy(message, "Hello from P1!");

        MPI_Isend(message, strlen(message), MPI_INT, rank-1, 7, MPI_COMM_WORLD, &request);
        MPI_Irecv(message, strlen(message), MPI_INT, rank-1, 7, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        
        printf("\nR1 Received: %s\n\n", message);
    }
    
    MPI_Finalize();
    return 0;
}
