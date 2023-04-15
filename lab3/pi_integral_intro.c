

/* ======================================================================== */
/*   pi.c                                                                   */
/*   MPI program for calculating Pi by numerical integration                */
/*   - basic code framework                                                 */
/* ======================================================================== */
#include "mpi.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    long n = 0;
    int myid = 0;
    int numprocs = 1;
    int i;
    double PI25DT = 3.141592653589793238462643;
    long double mypi;
    long double pi;
    long double h;
    long double sum;
    long double x;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    /***
     * Calculate pi by numerical integration of a arctangent derivative function.
     * Read a number of stripes (rectangles or trapezes) under
     * the function curve in every loop iteration,
     */

    while (1)
    {
        if (myid == 0)
        {
            printf("Enter the number of intervals: (0 quits) ");
            scanf("%ld", &n);
        }

        // MPI_...
        MPI_Bcast(&n, 1, MPI_LONG, 0, MPI_COMM_WORLD);

        if (n == 0)
        {
            break;
        }

        h = 1.0 / (long double)n;
        sum = 0.0;

        for (i = myid + 1; i <= n; i += numprocs)
        {
            x = h * ((long double)i - 0.5);
            sum += (4.0 / (1.0 + x * x));
        }
        mypi = h * sum;

        pi = mypi; /* !!! seriously? serially?  */

        //     MPI_...
        MPI_Reduce(&mypi, &pi, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (myid == 0)
        {
            printf("pi is approximately %.23Lf, Error is %.23Lf\n", pi, fabsl(pi - PI25DT));
        }
    } // while

    MPI_Finalize();
    return 0;
}

// mpicc pi_integral_intro.c -Wall -pedantic
// mpiexec -f nodes -n 30 ./a.out
