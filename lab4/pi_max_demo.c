

/* ---------------------------------------------------------------- *
 *  pi_max.c - MPI Pi calculation by dart throwing simulation       *
 *	    	     A framework for student lab  		    *
 * ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define DARTS 10000000 /* number of throws at the dartboard */
#define ROUNDS 20      /* number of times "darts" throwing is iterated */
#define MASTER 0       /* task ID of master task */
#define sqr(x) ((x) * (x))

long random(void);

/* Local Pi estimation */
double dboard(long darts) /* number of darts to throw */
{
    double x_coord;       /* x coordinate, between -1.0 and 1.0  */
    double y_coord;       /* y coordinate, between -1.0 and 1.0  */
    double pi;            /* Pi */
    double r;             /* random number between 0 and 1  */
    long score = 0l;      /* number of darts that hit circle */
    unsigned long cconst; /* used to convert integer random number
                           * between 0 and 2^31 to double random number
                           * between 0 and 1  */

    cconst = RAND_MAX; /* 2 << (31 - 1) */

    /* "throw the darts at the board" */

    for (long n = 0; n < darts; n++)
    {

        /* generate random numbers for x and y coordinates */

        r = (double)random() / cconst;
        x_coord = (2.0 * r) - 1.0;
        r = (double)random() / cconst;
        y_coord = (2.0 * r) - 1.0;

        /* if the dart lands in a circle, increment score */

        if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
        {
            score++;
        }
    }

    /* calculate Pi */
    pi = 4.0 * (double)score / (double)darts;
    return (pi);
}

typedef struct ValueWithLocation
{
    double val;
    int pos;
} ValueWithLocation;

/* MPI-based Pi aggregation */
int main(int argc, char **argv)
{
    double homepi; /* value of Pi calculated by current local task */
    double pisum;  /* sum of tasks' Pi values */
    double pi;     /* average of Pi after all "darts" are thrown */
    double avepi;  /* average Pi value for all iterations */
    int mytid;     /* task ID - also used as seed number */
    int nproc;     /* number of tasks */
    int rcode;     /* return code */
    int i;
    ValueWithLocation maxloc, minloc, locpair;

    /* Obtain number of tasks and task ID */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mytid);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    printf("MPI task ID = %d\n", mytid);

    /* Set seed for random number generator equal to task ID */
    srandom(mytid);

    avepi = 0.0;
    for (i = 0; i < ROUNDS; i++)
    {
        /* All tasks calculate Pi using dartboard algorithm */
        homepi = dboard(DARTS);

        /* Use MPI_Reduce to sum values of homepi across all tasks
         * Master will store the accumulated value in pisum
         * - homepi is the send buffer
         * - pisum is the receive buffer (used by the receiving task only)
         * - the size of the message is sizeof(double)
         * - MASTER is the task that will receive the result of the reduction
         *   operation
         */

        /* Calculate a sum of Pi values from all MPI processes */
        MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

        // Find MPI process with biggest Pi estimation
        // locpair.val =  ... ; locpair.pos = ... ;
        // MPI_Reduce(&locpair, &maxloc, ... );

        // Find MPI process with smallest Pi estimation
        // locpair.val = ... ; locpair.pos = ... ;
        // MPI_Reduce(&locpair, &minloc, ... );

        // Find MPI process with biggest Pi estimation
        locpair.val = homepi;
        locpair.pos = mytid;
        MPI_Reduce(&locpair, &maxloc, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MASTER, MPI_COMM_WORLD);
        // Find MPI process with smallest Pi estimation
        locpair.val = homepi;
        locpair.pos = mytid;
        MPI_Reduce(&locpair, &minloc, 1, MPI_DOUBLE_INT, MPI_MINLOC, MASTER, MPI_COMM_WORLD);

        /* Master computes average for this iteration and all iterations */
        if (mytid == MASTER)
        {
            pi = pisum / nproc;
            avepi = ((avepi * i) + pi) / (i + 1);
            printf(" In round %d, current value of pi = %23.20f\n", (i + 1), pi);
            printf(" After %d throws, average value of pi = %23.20f\n", (DARTS * (i + 1)), avepi);

            // printf("(max[%d] -> %12.8f) - (min[%d] -> %12.8f) = %12.8f\n",
            // maxloc.pos, maxloc.val, minloc.pos, minloc.val, maxloc.val-minloc.val);

            printf("(max[%d] -> %12.8f) - (min[%d] -> %12.8f) = %12.8f\n",
                   maxloc.pos, maxloc.val, minloc.pos, minloc.val, maxloc.val - minloc.val);
        }
    }
    MPI_Finalize();
    // MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Abort(MPI_COMM_WORLD, 0);
    return 0;
}
