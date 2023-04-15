/*
 *  	MPI Hello World
 */

#include "mpi.h"
#include <stdio.h>

typedef struct mpi_data
{
    int myid;
    int numprocs;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
} mpi_data;

int main(int argc, char *argv[])
{
    mpi_data proc_data;
    int is_success = MPI_Init(&argc, &argv);
    // if(is_success != MPI_SUCCESS)
    // {
    //     printf("oh no");
    //     return 0;
    // }
    // else
    // {
    //     printf("Hell yes");
    // }

    MPI_Comm_size(MPI_COMM_WORLD, &proc_data.numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_data.myid); // zwroc moj numer w jakims komunikatorze (tutaj w globalnym)
    MPI_Get_processor_name(proc_data.processor_name, &proc_data.namelen);

    fprintf(stdout, "Hello, world! This is MPI process %d of %d on node %s\n",
            proc_data.myid, proc_data.numprocs, proc_data.processor_name);
    fflush(stdout);

    MPI_Finalize();
    return 0;
}
