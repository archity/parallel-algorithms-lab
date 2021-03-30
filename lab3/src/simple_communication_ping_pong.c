#include <stdio.h>
#include <stdint.h>
#include "mpi.h"

int main(int argc, char** argv){

    int rank, size;
    MPI_Status status0, status1;

    // Prepare the number array
    uint64_t numb[10];
    for(int i = 0; i < 10; i++)
    {
        numb[i] = (uint64_t)i;
    }

    uint64_t recv_array[10];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Sender sends the array
    if(rank == 0)
    {
        MPI_Send(numb, 10, MPI_UNSIGNED_LONG_LONG, 1, 7, MPI_COMM_WORLD);
        MPI_Recv(numb, 10, MPI_UNSIGNED_LONG_LONG, 1, 7, MPI_COMM_WORLD, &status0);
        printf("\nR0 received this back from R1: \n");
        for(int i = 0; i < 10; i++)
        {
            printf("%ld, ", numb[i]);
        }
        printf("\n");
    }
    // Receiver receives the array, prints it, and sends it back after modifying it.
    else if (rank == 1)
    {
        MPI_Recv(recv_array, 10, MPI_UNSIGNED_LONG_LONG, 0, 7, MPI_COMM_WORLD, &status1);
        printf("\nR1 received this from R0: \n");
        for(int i = 0; i < 10; i++)
        {
            printf("%ld, ", recv_array[i]);
            recv_array[i] = recv_array[i] * recv_array[i];
        }
        MPI_Send(recv_array, 10, MPI_UNSIGNED_LONG_LONG, 0, 7, MPI_COMM_WORLD);
        printf("\nModifications sent back to R0\n\n");
    }
    
    MPI_Finalize();
    return 0;
}
