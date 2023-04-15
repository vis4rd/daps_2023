

/* ======================================================================== */
/*   mpe_pi_X11.c                                                           */
/*   MPI program for calculating Pi by Monte Carlo estimation with MPEgraph */
/* ======================================================================== */
/***
   * Lab. Stud.204 X11-enabled desktop MPICH-4 compilation:
   *
    $ /opt/nfs/mpich-4.1.1/bin/mpicc -o mpe_pi_X11 mpe_pi_X11.c      \
        -I/opt/nfs/mpe2-2.4.9b/include -L/opt/nfs/mpe2-2.4.9b/lib  \
        -lmpe -lX11 -lm

   * Lab. Stud.204 X11-enabled desktop MPICH-4 example execution:
   ***
    $
    $ ssh -X $( hostname )
    $ Xephyr -ac -geometry 1280x1024 -listen tcp -resizeable :1 &
    $ xterm -fn 10x20 -display :1 &
   ***  (xterm prompt in Xephyr window) ***
    $ mate-wm &
    $ opt/nfs/config/station204_name_list.sh 1 16 > nodes
    $ /opt/nfs/mpich-4.1.1/bin/mpiexec -f nodes -n $(( 1 * $(cat nodes | wc -l) + 1 )) \
        -env DISPLAY $( hostname ):1 ./mpe_pi_X11 0.00001

   ***
   */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#include "mpe.h"
#include "mpe_graphics.h"
#define CHUNKSIZE 10000
/* We'd like a value that gives the maximum value returned by the function
   random, but no such value is *portable*.  RAND_MAX is available on many
   systems but is not always the correct value for random (it isn't for
   Solaris).  The value ((unsigned(1)<<31)-1) is common but not guaranteed */
#define THROW_MAX 1000000000

/* message tags */
#define REQUEST 1
#define REPLY 2
#define WINDOW_SIZE 400
int main(int argc, char *argv[])
{
    int iter;
    int in, out, i, iters, max, ix, iy, ranks[1], done, temp;
    double x, y, Pi, error, epsilon;
    int numprocs, myid, server, totalin, totalout, workerid;
    int rands[CHUNKSIZE], request;
    MPI_Comm world, workers;
    MPI_Group world_group, worker_group;
    MPI_Status status;

    /* declare MPE graphics "object" */
    /* ....Fill in, please.... */

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numprocs);
    MPI_Comm_rank(world, &myid);
    server = numprocs - 1; /* last proc is server */
    epsilon = 0.0001;      /* example */
    if (myid == 0)
    { /* read epsilon from command line */
        if (argv[1] != NULL)
            sscanf(argv[1], "%lf", &epsilon);
    }
    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Comm_group(world, &world_group);
    ranks[0] = server;
    MPI_Group_excl(world_group, 1, ranks, &worker_group);
    MPI_Comm_create(world, worker_group, &workers);
    MPI_Group_free(&worker_group);
    MPI_Group_free(&world_group);

    /* open MPE graphics window */
    /* ....Fill in, please.... */

    if (myid == server)
    { /* I am the rand server */
        do
        {
            MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST,
                     world, &status);
            if (request)
            {
                for (i = 0; i < CHUNKSIZE; i++)
                {
                    rands[i] = random();
                }
                MPI_Send(rands, CHUNKSIZE, MPI_INT,
                         status.MPI_SOURCE, REPLY, world);
            }
        } while (request > 0);
    }
    else
    { /* I am a worker process */
        request = 1;
        done = in = out = 0;
        max = RAND_MAX; /* max int, for normalization */
        MPI_Send(&request, 1, MPI_INT, server, REQUEST, world);
        MPI_Comm_rank(workers, &workerid);
        iter = 0;
        while (!done)
        {
            iter++;
            request = 1;
            MPI_Recv(rands, CHUNKSIZE, MPI_INT, server, REPLY,
                     world, &status);
            for (i = 0; i < CHUNKSIZE - 1;)
            {
                x = (((double)rands[i++]) / max) * 2 - 1;
                y = (((double)rands[i++]) / max) * 2 - 1;
                if (x * x + y * y < 1.0)
                {
                    in++;

                    /* draw point in MPE graphics window */
                    /* ....Fill in, please.... */
                }
                else
                    out++;
            }
            MPI_Allreduce(&in, &totalin, 1, MPI_INT, MPI_SUM,
                          workers);
            MPI_Allreduce(&out, &totalout, 1, MPI_INT, MPI_SUM,
                          workers);
            Pi = (4.0 * totalin) / (totalin + totalout);

            /* update content of MPE graphics window */
            /* ....Fill in, please.... */

            error = fabs(Pi - 3.141592653589793238462643);
            done = (error < epsilon || (totalin + totalout) > THROW_MAX);
            request = (done) ? 0 : 1;
            if (myid == 0)
            {
                printf("\rpi = %.7f", Pi);
                MPI_Send(&request, 1, MPI_INT, server, REQUEST,
                         world);
            }
            else
            {
                if (request)
                    MPI_Send(&request, 1, MPI_INT, server, REQUEST,
                             world);
            }
        }
        MPI_Comm_free(&workers);
    }

    if (myid == 0)
    {
        printf("\npoints: %d\nin: %d, out: %d, press  to exit\n",
               totalin + totalout, totalin, totalout);
        getchar();
    }
    MPI_Barrier(MPI_COMM_WORLD);

    /* close MPE graphics "object" */
    /* ....Fill in, please.... */

    MPI_Finalize();
}
