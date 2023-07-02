/*
 * Calculate Pi using OpenMP threads.
 * Store private results in separate shared table elements.
 *
 Compile:
  gcc -O2 -fopenmp -o pi-table pi-table.c

 Setup:
  export OMP_NUM_THREADS=4

 Run:
  ./pi-table

 *
 **/
#include <omp.h>
#include <stdio.h>
// #define NUM_THREADS 4

static long num_steps = 1000000000;
double step;

void main()
{
#ifndef NUM_THREADS
#define NUM_THREADS 20
#endif
    int i, nthreads;
    double pi, sum[NUM_THREADS];

    double start_time, run_time;
    start_time = omp_get_wtime();

    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);

// parallel block
#pragma omp parallel
    {
        int i, id, nthrds;
        double x;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        if (id == 0)
            nthreads = nthrds;
        for (i = id, sum[id] = 0.0; i < num_steps; i += nthrds)
        {
            x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    } // end of parallel block

    for (i = 0, pi = 0.0; i < nthreads; i++)
        pi += sum[i] * step;
    printf("\n num_steps: %ld nthreads: %d Pi: %.16f \n", num_steps, nthreads, pi);

    run_time = omp_get_wtime() - start_time;
    printf("\n OpenMP runtime: %f seconds \n", run_time);
}
