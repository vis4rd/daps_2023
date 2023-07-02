/*
 * Calculate Pi using OpenMP threads.
 * Recursively schedule subtasks to share calculations among them.
 * OpenMP 3.x support needed.
 *
 Compile:
  gcc -O2 -fopenmp -o pi-tasks pi-tasks.c

 Setup:
  export OMP_NUM_THREADS=4

 Run:
  ./pi-tasks

 *
 */
#include <omp.h>
#include <stdio.h>
// #define NUM_THREADS 4

static long num_steps = 1000000000;
double step;

#define MIN_BLK 1000000
double pi_comp(int Nstart, int Nfinish, double step)
{
    int i, iblk;
    double x, sum = 0.0, sum1, sum2;

    if (Nfinish - Nstart < MIN_BLK)
    {
        // final depth - calculate partial result and return
        for (i = Nstart; i < Nfinish; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    }
    else
    {
        // recursion - spawn more sub-tasks
        iblk = Nfinish - Nstart;

// start subordinate task (for 1st data half)
#pragma omp task shared(sum1)
        sum1 = pi_comp(Nstart, Nfinish - iblk / 2, step);

// start subordinate task (for 2nd data half)
#pragma omp task shared(sum2)
        sum2 = pi_comp(Nfinish - iblk / 2, Nfinish, step);

// wait for completion of subordinate tasks
#pragma omp taskwait

        sum = sum1 + sum2;
    }
    return sum;
}

void main()
{
    int i, nthrds;
    double step, pi, sum;

    double start_time, run_time;
    start_time = omp_get_wtime();

    step = 1.0 / (double)num_steps;
// omp_set_num_threads(NUM_THREADS);

// parallel block
#pragma omp parallel
    {
// master thread updates shared variable
#pragma omp master
        nthrds = omp_get_num_threads();

// quickest thread starts single superior task
#pragma omp single
        sum = pi_comp(0, num_steps, step);

    } // end of parallel block

    pi = step * sum;
    printf("\n num_steps: %ld nthreads: %d Pi: %.16f \n", num_steps, nthrds, pi);

    run_time = omp_get_wtime() - start_time;
    printf("\n OpenMP runtime: %f seconds \n", run_time);
}
