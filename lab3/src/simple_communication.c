#include <stdio.h>
#include <stdint.h>
#include "mpi.h"

int main(int argc, char** argv){

    int rank, size;
     MPI_Status status;

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
    }
    //Receiver receives the array, and prints it
    else if (rank == 1)
    {
        MPI_Recv(recv_array, 10, MPI_UNSIGNED_LONG_LONG, 0, 7, MPI_COMM_WORLD, &status);
        printf("Rank 1 recived this from rank 0: \n");
        for(int i = 0; i < 10; i++)
        {
            printf("%ld, ", recv_array[i]);
        }
        printf("\n");
    }
    
    MPI_Finalize();
    return 0;
}
