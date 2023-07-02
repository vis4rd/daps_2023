/*
 * Calculate Pi using OpenMP threads.
 * Use reduction clause to get global result.
 *
 Compile:
  gcc -O2 -fopenmp -o pi-reduction pi-reduction.c

 Setup:
  export OMP_NUM_THREADS=4

 Run:
  ./pi-reduction

 *
 */
#include <stdio.h>
#include <omp.h>
// #define NUM_THREADS 2

static long num_steps = 1000000000;
double step;

void main()
{
#ifndef NUM_THREADS
#define NUM_THREADS 20
#endif
    int i, nthreads;
    double x, pi, sum = 0.0;

    double start_time, run_time;
    start_time = omp_get_wtime();

    step = 1.0 / (double)num_steps;
// omp_set_num_threads (NUM_THREADS);

// parallel and for block - merged together
#pragma omp parallel for private(x) reduction(+ \
                                              : sum)
    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
        if (i == 0)
            nthreads = omp_get_num_threads();
    } // end of parallel and for block

    pi = step * sum;

    printf("\n num_steps: %ld nthreads: %d Pi: %.16f \n", num_steps, nthreads, pi);

    run_time = omp_get_wtime() - start_time;
    printf("\n OpenMP runtime: %f seconds \n", run_time);
}
