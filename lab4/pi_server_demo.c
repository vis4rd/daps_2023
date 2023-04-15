

/* ======================================================================== */
/*   pi_server.c                                                            */
/*   MPI program for calculating Pi by Monte Carlo estimation               */
/*   with separate RNG - basic code framework                               */
/* ======================================================================== */
/***
 * Stud. lab.204 MPICH compilation:
 *   /opt/nfs/mpich-4.0.1/bin/mpicc -o pi_server pi_server.c -lm
 * Stud. lab.204 MPICH example execution:
 *   /opt/nfs/mpich-4.0.1/bin/mpiexec -n 4 ./pi_server 0.000001
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define CHUNKSIZE 10000
/* We'd like a value that gives the maximum value returned by the function
   random, but no such value is *portable*.  RAND_MAX is available on many
   systems but is not always the correct value for random (it isn't for
   Solaris).  The value ((unsigned(1)<<31)-1) is common but not guaranteed */
#define THROW_MAX 100000000
#define PI 3.141592653589793238462643
/* message tags */
#define REQUEST 1
#define REPLY 2

int main(int argc, char *argv[])
{
    int numprocs;
    int myid;
    int server;
    int workerid;
    int ranks[1];
    int request;
    int i;
    int iter;
    int ix;
    int iy;
    int done;
    long rands[CHUNKSIZE];
    long max;
    long in;
    long out;
    long totalin;
    long totalout;
    double x;
    double y;
    double Pi;
    double error;
    double epsilon;
    MPI_Comm world;
    MPI_Comm workers;
    MPI_Group world_group;
    MPI_Group worker_group;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numprocs);
    MPI_Comm_rank(world, &myid);
    server = numprocs - 1; // Last process is a random server

    /***
     * Now Master should read epsilon from command line
     * and distribute it to all processes.
     */

    /* ....Fill in, please.... */

    if (myid == 0) // Read epsilon from command line
    {
        sscanf(argv[1], "%lf", &epsilon);
    }
    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, world);

    /***
     * Create new process group called world_group containing all
     * processes and its communicator called world
     * and a group called worker_group containing all processes
     * except the last one (called here server)
     * and its communicator called workers.
     */

    /* ....Fill in, please.... */

    /*  Use MPI group and commmunicator operations.... */

    //    MPI_Comm_group( world, .... );
    //    ranks[0] = server;
    //    MPI_Group_excl( world_group, 1, .... );
    //    MPI_Comm_create( world, .... );

    MPI_Comm_group(world, &world_group);
    ranks[0] = server;
    MPI_Group_excl(world_group, 1, ranks, &worker_group);
    MPI_Comm_create(world, worker_group, &workers);
    MPI_Group_free(&worker_group);
    MPI_Group_free(&world_group);

    /***
     * Server part
     *
     * Server should loop until request code is 0, in each iteration:
     * - receiving request code from any slave
     * - generating a vector of CHUNKSIZE randoms
     * - sending vector back to slave
     */
    if (myid == server)
    { // I am the random generator server

        /* ....Fill in, please.... */

        /*
                ....On request from a worker generate a bunch of random numbers
                and send them back...
        */
        do
        {
            MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST, world, &status);
            if (request)
            {
                for (i = 0; i < CHUNKSIZE; i++)
                {
                    rands[i] = random();
                }
                MPI_Send(rands, CHUNKSIZE, MPI_LONG, status.MPI_SOURCE, REPLY, world);
            }
        } while (request > 0);
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
    else
    { // I am a worker process

        /* ....Fill in, please.... */

        /*
                    ... Loop until error < epsilon or total > THROW_MAX

                    ... throw number of darts
                    ... calculate Pi globally
                    ... test epsilon condition
                    ... decide, should server be closed?
               ... Free resources not needed
        */

        request = 1;
        done = 0;
        in = out = 0;
        max = RAND_MAX; // max int, for normalization
        MPI_Send(&request, 1, MPI_INT, server, REQUEST, world);
        MPI_Comm_rank(workers, &workerid);
        iter = 0;
        while (!done)
        {
            iter++;
            request = 1;
            MPI_Recv(rands, CHUNKSIZE, MPI_LONG, server, REPLY,
                     world, &status);
            for (i = 0; i < CHUNKSIZE - 1;)
            {
                x = (((double)rands[i++]) / max) * 2 - 1;
                y = (((double)rands[i++]) / max) * 2 - 1;
                if (x * x + y * y < 1.0)
                {
                    in++;
                }
                else
                {
                    out++;
                }
            }

            // Workers update total statistics
            MPI_Allreduce(&in, &totalin, 1, MPI_LONG, MPI_SUM, workers);
            MPI_Allreduce(&out, &totalout, 1, MPI_LONG, MPI_SUM, workers);
            // Testing for final results
            Pi = (4.0 * totalin) / (totalin + totalout);
            error = fabs(Pi - PI);
            done = (error < epsilon || (totalin + totalout) > THROW_MAX);
            request = (done) ? 0 : 1;
            // Master must send next request, other workers may not
            if (myid == 0)
            {
                printf("\rpi = %23.20f", Pi);
                MPI_Send(&request, 1, MPI_INT, server, REQUEST, world);
            }
            else
            {
                if (request)
                {
                    MPI_Send(&request, 1, MPI_INT, server, REQUEST, world);
                }
            }
        }

        // Workers no longer needed
        /* ....Fill in, please.... */
        MPI_Comm_free(&workers);

    } // End of worker (including master) part

    /***
     * Master should print final point counts.
     */

    /* ....Fill in, please.... */
    if (myid == 0)
    {
        printf("\npoints: %d\nin: %d, out: %d,  to exit\n", totalin + totalout, totalin, totalout);
        getchar();
    }

    /***
     * End of MPI operations.
     */

    MPI_Finalize();
}
