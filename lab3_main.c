#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void print_matrix(int row, int col, int* matrix)
{
    int i, j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            printf("%5d ", matrix[i * col + j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Put in args size of matrix\n");
        return 0;
    }
    int row, col = atoi(argv[1]);

    int rank, size, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Get_processor_name(processor_name, &namelen);

    printf("Process %d of %d runned on %s\n", rank, size, processor_name);
    int i, j, count;
    int* a = (int*)calloc(n * n, sizeof(int));
    int *b, *blocklen, *index;
    if (rank != 0) {
        b = (int*)calloc(n * (n + 1) / 2, sizeof(int));
    }
    blocklen = (int*)calloc(n, sizeof(int));
    for (i = n; i >= 0; i--) {
        blocklen[i] = n - i;
    }
    int ind = 0;
    index = (int*)calloc(n, sizeof(int));
    for (i = 0; i < n; i++) {        
        index[i] = i * n + ind;
        ind++;
    }
    MPI_Datatype newtype;
    MPI_Type_indexed(n, blocklen, index, MPI_INT, &newtype);
    MPI_Type_commit(&newtype);

    if (rank == 0) {
        count = 0;
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                a[i * n + j] = ++count;
            }
        }
    }
    if (rank == 0) {
        printf("Before send matrix A process %d:\n", rank);
        print_matrix(n, n, a);
        MPI_Send(a, 1, newtype, 1, 1, MPI_COMM_WORLD);
        MPI_Send(a, 1, newtype, 1, 2, MPI_COMM_WORLD);
    }
    else
        {
            MPI_Recv(a, 1, newtype, 0, 1, MPI_COMM_WORLD, 0);
            MPI_Recv(b, n * (n + 1) / 2, MPI_INT, 0, 2, MPI_COMM_WORLD, 0);
        }
    if (rank != 0) {
        printf("After receive matrix A process %d:\n", rank);

        print_matrix(n, n, a);
    }
    if (rank != 0) {
        printf("After receive matrix B process %d:\n", rank);

        int k = 0;
        for (i = 0; i < n; i++) {
            for (j = n; j > i; j--){
                if(i != 0 && j == n)
                    for(int z = 0; z < i; z++)
                    {
                        printf("%6s", " ");
                    }
                printf("%5d ", b[k++]);
            }
            printf("\n");
        }
        printf("\n");

    }
    MPI_Type_free(&newtype);
    free(a);
    if (rank != 0)
        free(b);
    MPI_Finalize();
    return 0;
}