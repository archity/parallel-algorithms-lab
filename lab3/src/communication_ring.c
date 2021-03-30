#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char** argv){

    int rank, size;
    int token;

    /* Intializes random number generator */
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status[size];

    // Sender sends the array
    if(rank == 0)
    {
        token = 0;
        printf("R0: %d\n", token);
        token += rand() % 100;
        printf("Modified to: %d\n\n", token);
        MPI_Send(&token, 1, MPI_INT, rank+1, 7, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_INT, size-1, 7, MPI_COMM_WORLD, &status[0]);
        printf("\nR0 Received back: %d\n\n", token);
    }
    
    else if (rank == 1)
    {
        MPI_Recv(&token, 1, MPI_INT, rank-1, 7, MPI_COMM_WORLD, &status[1]);
        printf("R1:\n");
        printf("Received: %d\n", token);
        token += rand() % 100;
        printf("Modified to: %d\n\n", token);
        MPI_Send(&token, 1, MPI_INT, rank+1, 7, MPI_COMM_WORLD);
    }
    else if (rank == 2)
    {
        MPI_Recv(&token, 1, MPI_INT, rank-1, 7, MPI_COMM_WORLD, &status[2]);
        printf("R2:\n");
        printf("Received: %d\n", token);
        token += rand() % 100;
        printf("Modified to: %d\n\n", token);
        MPI_Send(&token, 1, MPI_INT, rank+1, 7, MPI_COMM_WORLD);
    }
    else if (rank == 3)
    {
        MPI_Recv(&token, 1, MPI_INT, rank-1, 7, MPI_COMM_WORLD, &status[3]);
        printf("R3:\n");
        printf("Received: %d\n", token);
        token += rand() % 100;
        printf("Modified to: %d\n\n", token);
        MPI_Send(&token, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
