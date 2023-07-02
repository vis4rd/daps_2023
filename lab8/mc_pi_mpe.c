

/* ======================================================================== */
/*   mc_pi_mpe.c  - log MPI operations to file using profiling interface    */
/*   MPI program for calculating Pi by Monte Carlo estimation with MPE      */
/*                        D-10 Lab.204 MPICH-4/MPE                          */
/* ======================================================================== */
/***
   * Compilation:
   *
    $ /opt/nfs/mpich-4.1.1/bin/mpicc mpicc -o mc_pi_mpe mc_pi_mpe.c  \
        -I/opt/nfs/mpe2-2.4.9b/include -L/opt/nfs/mpe2-2.4.9b/lib    \
        -lmpe -lm -lpthread
   *
   * Example execution:
   *
    $ /opt/nfs/config/station204_name_list.sh 1 16 > nodes
    $ /opt/nfs/mpich-4.1.1/bin/mpiexec -f nodes -n $(( 1 * $(cat nodes | wc -l) )) \
        ./mc_pi_mpe 0.00001
   *
   * Log conversion:
   *
    $ /opt/nfs/mpe2-2.4.9b/bin/clog2TOslog2 mpe-log-file.clog2
   *
   * Display log on X11 desktop GUI using Jumpshot:
   *
    $ /opt/nfs/mpe2-2.4.9b/bin/jumpshot mpe-log-file.slog2
   *
   *** Jumpshot manual:
    $ firefox /opt/nfs/mpe2-2.4.9b/share/doc/jumpshot-4/html/index.html
   *** or PDF ***
    $ firefox /opt/nfs/mpe2-2.4.9b/share/doc/jumpshot-4/usersguide.pdf
   *
   ***/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

#include "mpe.h"

/* RN vector size */
#define CHUNKSIZE 10000

/* We'd like a value that gives the maximum value returned by the function
   random, but no such value is *portable*.  RAND_MAX is available on many
   systems but is not always the correct value for random (it isn't for
   Solaris).  The value ((unsigned(1)<<31)-1) is common but not guaranteed */
#define INT_MAX 1000000000

/* message tags */
#define REQUEST 1
#define REPLY 2

/************************************************************/

// This is an example of MPI profiling interface

#define START_BCAST 0  // Define boundary events
#define END_BCAST 1    // ...
#define START_ALLRED 2 // for each communication operation
#define END_ALLRED 3   // ...
#define START_RECV 4   // to be logged
#define END_RECV 5     // ...
#define START_SEND 6
#define END_SEND 7

int event1, event2; // and for some other purpose

double startwtime = 0.0, endwtime; // Time measurements

MPE_LOG_BYTES bytebuf; // Allocate log data buffer
int bytebuf_pos;       // and buffer fill position

// ... to be continued ...

int MPI_Init(int *argc, char ***argv)
{

    int ret, myid;
    ret = PMPI_Init(argc, argv);          // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Who am I globally
    MPE_Init_log();                       // Initialize MPE logging

    MPE_Log_get_solo_eventID(&event1); // Get event numbers for solo
    MPE_Log_get_solo_eventID(&event2); // events (single timestamp)

    if (myid == 0) // Define log blocks
    {
        MPE_Describe_info_state(START_SEND, END_SEND, "send", "blue", "Destination id =  %d.");
        MPE_Describe_info_state(START_RECV, END_RECV, "receive", "yellow", "Source id =  %d.");
        MPE_Describe_info_state(START_BCAST, END_BCAST, "broadcast", "red", "Root id = %d.");
        MPE_Describe_info_state(START_ALLRED, END_ALLRED, "reduction", "green", "Op count = %d.");

        MPE_Describe_event(event1, "Computation Start", "purple");
        MPE_Describe_event(event2, "Computation End", "navy");

        startwtime = MPI_Wtime(); // Start MPI time period
    }

    MPI_Barrier(MPI_COMM_WORLD); // Wait for master process

    MPE_Start_log(); // Start MPE logging

    return ret;
}

int MPI_Finalize(void)
{

    int ret, myid;

    MPE_Log_sync_clocks();                // Synchronize clocks
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); // Who am I globally

    if (myid == 0)
    {
        endwtime = MPI_Wtime(); // End MPI time period
        printf("Wall clock time elapsed = %f\n", endwtime - startwtime);
    }

    MPE_Finish_log("mpe-profile"); // Dump to CLOG2 file

    ret = PMPI_Finalize(); // Disconnect MPI

    return ret;
}

int MPI_Bcast(void *buf, int count, MPI_Datatype datatype,
              int root, MPI_Comm comm)
{

    int ret, myid;
    // (not used) MPI_Comm_rank( comm, &myid );				// Who am I

    MPE_Log_comm_event(MPI_COMM_WORLD, START_BCAST, NULL); // Log block

    ret = PMPI_Bcast(buf, count, datatype, root, comm); // Communicate

    bytebuf_pos = 0;
    MPE_Log_pack(bytebuf, &bytebuf_pos, 'd', 1, &root);     // Pack data
    MPE_Log_comm_event(MPI_COMM_WORLD, END_BCAST, bytebuf); // Log block

    return ret;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,
             MPI_Comm comm)
{

    int ret, myid;
    // (not used) MPI_Comm_rank( comm, &myid );			// Who am I

    MPE_Log_comm_event(MPI_COMM_WORLD, START_SEND, NULL); // Log block

    ret = PMPI_Send(buf, count, datatype, dest, tag, comm); // Communicate

    MPE_Log_send(dest, tag, sizeof(buf)); // Log arrow

    bytebuf_pos = 0;
    MPE_Log_pack(bytebuf, &bytebuf_pos, 'd', 1, &dest);    // Pack data
    MPE_Log_comm_event(MPI_COMM_WORLD, END_SEND, bytebuf); // Log block

    return ret;
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
             MPI_Comm comm, MPI_Status *status)
{

    int ret, myid;
    // (not used) MPI_Comm_rank( comm, &myid );			// Who am I

    MPE_Log_comm_event(MPI_COMM_WORLD, START_RECV, NULL); // Log block

    ret = PMPI_Recv(buf, count, datatype, source, tag, comm, // Communicate
                    status);

    MPE_Log_receive(source, tag, sizeof(buf)); // Log arrow

    bytebuf_pos = 0;
    MPE_Log_pack(bytebuf, &bytebuf_pos, 'd', 1, &source);  // Pack data
    MPE_Log_comm_event(MPI_COMM_WORLD, END_RECV, bytebuf); // Log block

    return ret;
}

int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count,
                  MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{

    int ret, myid;
    // (not used) MPI_Comm_rank( comm, &myid );			// Who am I

    MPE_Log_comm_event(MPI_COMM_WORLD, START_ALLRED, NULL); // Log block

    ret = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, // Communicate
                         comm);

    bytebuf_pos = 0;
    MPE_Log_pack(bytebuf, &bytebuf_pos, 'd', 1, &count);     // Pack data
    MPE_Log_comm_event(MPI_COMM_WORLD, END_ALLRED, bytebuf); // Log block

    return ret;
}

/************************************************************/

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

    MPI_Init(&argc, &argv);

    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numprocs);
    MPI_Comm_rank(world, &myid);
    server = numprocs - 1; /* last proc is server */

    /***
     * Now Master should read epsilon from command line
     * and distribute it to all processes.
     */
    if (myid == 0) /* read epsilon from command line */
        sscanf(argv[1], "%lf", &epsilon);

    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /***
     * Create new process group called world_group containing all
     * processes and its communicator called world
     * and a group called worker_group containing all processes
     * except the last one (called here server)
     * and its communicator called workers.
     */
    MPI_Comm_group(world, &world_group);
    ranks[0] = server;
    MPI_Group_excl(world_group, 1, ranks, &worker_group);
    MPI_Comm_create(world, worker_group, &workers);
    MPI_Group_free(&worker_group);
    MPI_Group_free(&world_group);

    // Solo log event - start of calculations
    MPE_Log_event(event1, server, NULL);

    /***
     * Server part
     *
     * Server should loop until request code is 0, in each iteration:
     * - receiving request code from any slave
     * - generating a vector of CHUNKSIZE randoms <= INT_MAX
     * - sending vector back to slave
     */
    if (myid == server)
    { /* I am the rand server */
        do
        {

            MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST,
                     world, &status);

            if (request)
            {
                for (i = 0; i < CHUNKSIZE;)
                {
                    rands[i] = random();
                    if (rands[i] <= INT_MAX)
                        i++;
                }

                MPI_Send(rands, CHUNKSIZE, MPI_INT,
                         status.MPI_SOURCE, REPLY, world);
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
     * - calculate pi and its error (from "exact" value)
     * - test if error is within epsilon limit
     * - test continuation condition (error and max. points limit)
     * - print pi by master only
     * - send a request to server (all if more or master only if finish)
     * Before finishing workers should free their communicator.
     */
    else
    { /* I am a worker process */
        request = 1;
        done = in = out = 0;
        max = INT_MAX; /* max int, for normalization */

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
                }
                else
                    out++;
            }

            MPI_Allreduce(&in, &totalin, 1, MPI_INT, MPI_SUM,
                          workers);
            MPI_Allreduce(&out, &totalout, 1, MPI_INT, MPI_SUM,
                          workers);

            Pi = (4.0 * totalin) / (totalin + totalout);
            error = fabs(Pi - 3.141592653589793238462643);
            done = (error < epsilon || (totalin + totalout) > 1000000);
            request = (done) ? 0 : 1;
            if (myid == 0)
            {
                printf("\rpi = %23.20f", Pi);

                MPI_Send(&request, 1, MPI_INT, server, REQUEST,
                         world);
            }
            else
            {
                if (request)
                {
                    MPI_Send(&request, 1, MPI_INT, server, REQUEST,
                             world);
                }
            }
        }
        MPI_Comm_free(&workers);
    }

    // Solo log event - end of calculations
    MPE_Log_event(event2, done, NULL);

    /***
     * Master should print final point counts.
     */

    if (myid == 0)
    {
        printf("\npoints: %d\nin: %d, out: %d, <ret> to exit\n",
               totalin + totalout, totalin, totalout);
        getchar();
    }

    MPI_Finalize();
}
