/*
 * Calculate Pi using OpenMP threads.
 * Use critical section to protect shared variable.
 *
 Compile:
  gcc -O2 -fopenmp -o pi-critical pi-critical.c

 Setup:
  export OMP_NUM_THREADS=4

 Run:
  ./pi-critical

 *
 */
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
    double pi;

    double start_time, run_time;
    start_time = omp_get_wtime();

    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);

// parallel block
#pragma omp parallel
    {
        int i, id, nthrds;
        double x, sum;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        if (id == 0)
            nthreads = nthrds;
        for (i = id, sum = 0.0; i < num_steps; i = i + nthrds)
        {
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }

// critical section for shared memory update
#pragma omp critical
        pi += sum * step;

    } // end of parallel block

    printf("\n num_steps: %ld nthreads: %d Pi: %.16f \n", num_steps, nthreads, pi);

    run_time = omp_get_wtime() - start_time;
    printf("\n OpenMP runtime: %f seconds \n", run_time);
}
