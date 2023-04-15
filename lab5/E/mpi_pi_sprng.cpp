

/* ======================================================================== */
/*   mpi_pi_sprng.c                                                         */
/*   MPI program for calculating Pi by Monte Carlo estimation               */
/*   using SPRNG library		                                    */
/* ======================================================================== */
/***
   * Stud. lab.204 MPICH compilation:
   * /opt/nfs/mpich-4.0.1/bin/mpicxx -DSPRNG_MPI -DUSE_MPI -o mpi_pi_sprng mpi_pi_sprng.c \
    -I/opt/nfs/sprng5/include -L/opt/nfs/sprng5/lib -lsprng
   * Stud. lab.206 MPICH example execution:
   * /opt/nfs/mpich-4.0.1/bin/mpiexec -n 4 ./mpi_pi_sprng 0.000001
   */

#if __GNUC__ > 3
#include <string.h>
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdio.h>
#include <mpi.h> /* MPI header file    */
#include <math.h>
#include "sprng_cpp.h" /* SPRNG header file  */

#define SEED 985456376

#define CHUNKSIZE 1000000
#define THROW_MAX 100000000000
#define PI 3.141592653589793238462643

int main(int argc, char *argv[])
{
    int streamnum, nstreams, gtype;
    Sprng *stream;
    int numprocs, myid;
    long i, iter, done, in, out, totalin, totalout;
    double x, y, Pi, error, epsilon;
    MPI_Comm world;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numprocs);
    MPI_Comm_rank(world, &myid);

    /***
     * Now Master should read epsilon from command line
     * and distribute it to all processes.
     */

    /* ....Fill in, please.... */

    if (myid == 0) // Read epsilon from command line
        sscanf(argv[1], "%lf", &epsilon);
    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    streamnum = myid;
    nstreams = numprocs; /* one stream per processor                */
                         /*--- node 0 is reading in a generator type */
    if (myid == 0)
    {
#include <gen_types_menu.h>
        printf("Type in a generator type (integers: 0,1,2,3,4,5):  ");
        i = scanf("%d", &gtype);
    }

    MPI_Bcast(&gtype, 1, MPI_INT, 0, MPI_COMM_WORLD);
    stream = SelectType(gtype);
    stream->init_sprng(streamnum, nstreams, SEED, SPRNG_DEFAULT); /* initialize stream */
    if (myid == 0)
    {
        printf("\n\nProcess %d, print information about stream:\n", myid);
        stream->print_sprng();
    }

    /***
     * Workers (including Master) part
     *
     * Worker should send initial request to server.
     * Later, in a loop worker should:
     * - receive vector of randoms
     * - compute x,y point inside unit square
     * - check (and count result) if point is inside/outside
     *   unit circle
     * - sum both counts over all workers
     * - calculate Pi and its error (from "exact" value) in all workers
     * - test if error is within epsilon limit
     * - test continuation condition (error and max. points limit)
     * - print Pi by master only
     * - send a request to server (all if more or master only if finish)
     * Before finishing workers should free their communicator.
     */

    // I am a worker process

    done = 0;
    in = out = 0;
    iter = 0;
    while (!done)
    {
        iter++;
        // Insert dart throw loop here ...

        for (i = 0; i < CHUNKSIZE - 1; i += 2)
        {
            x = (stream->sprng()) * 2 - 1;
            y = (stream->sprng()) * 2 - 1;
            if (x * x + y * y < 1.0)
            {
                in++;
            }
            else
                out++;
        }

        // Workers update total statistics
        MPI_Allreduce(&in, &totalin, 1, MPI_LONG, MPI_SUM, world);
        MPI_Allreduce(&out, &totalout, 1, MPI_LONG, MPI_SUM, world);
        // Testing for final results
        Pi = (4.0 * totalin) / (totalin + totalout);
        error = fabs(Pi - PI);
        done = (error < epsilon || (totalin + totalout) > THROW_MAX);
        // Master prints pi
        if (myid == 0)
            if (iter % 100 == 0)
                printf("\rpi = %23.20f", Pi);
    }

    // End of worker (including master) part

    /***
     * Master should print final point counts.
     */

    if (myid == 0)
    {
        printf("\nPi = %23.20f", Pi);
        printf("\npoints: %ld\nin: %ld, out: %ld,  to exit\n",
               totalin + totalout, totalin, totalout);
        getchar();
    }

    /***
     * End of MPI operations.
     */

    stream->free_sprng(); /* free memory used to store stream state  */

    MPI_Finalize();

    return 0;
}
