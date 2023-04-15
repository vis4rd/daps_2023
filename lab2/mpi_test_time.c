
/* The following sample code illustrates MPI_Comm_get_attr. */

#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    void *v;
    int flag;
    int vval;
    int rank, size;
    double t1, t2;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    t1 = MPI_Wtime();

    MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &v, &flag);
    if (flag)
    {
        // Wtime need not be set
        vval = *(int *)v;
        if (vval < 0 || vval > 1)
        {
            fprintf(stderr, "Invalid value for WTIME_IS_GLOBAL (got %d)\n", vval);
            fflush(stderr);
        }
    }

    if (rank == 0)
    {
        printf("Press <enter> to exit\n");
        getchar();
    }

    MPI_Barrier(MPI_COMM_WORLD); // zatrzymaj inne procesy dopoki rank 0 nie dolaczy

    t2 = MPI_Wtime();
    printf("MPI_Wtime measured: %1.2f\n", t2 - t1);
    fflush(stdout);

    MPI_Finalize();

    return 0;
}
