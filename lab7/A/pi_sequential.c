/*
 * Calculate Pi - sequentially, using one OpenMP thread.
 *
 * This program will numerically compute the integral:
                   ^1
                  /  4 dx /( 1+ x*x )
        0v
  (from 0 to 1).  The value of this integral is Pi -- which
  is great since it gives us an easy way to check the answer.

 History: Written by Tim Mattson, 11/99.

 Compile:
  gcc -O2 -fopenmp -o pi-sequential pi-sequential.c

 Setup:
  export OMP_NUM_THREADS=4

 Run:
  ./pi-sequential

 *
 **/
#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
double step;

int main()
{
    int i;
    double x, pi, sum = 0.0;
    double start_time, run_time;
    start_time = omp_get_wtime();

    step = 1.0 / (double)num_steps;

    for (i = 1; i <= num_steps; i++)
    {
        x = (i - 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    pi = step * sum;
    printf("\n num_steps is %ld, pi is %.16f\n", num_steps, pi);
    run_time = omp_get_wtime() - start_time;
    printf("\n Runtime: %f seconds \n", run_time);
}
