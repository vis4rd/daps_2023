/*
 * Calculate Pi using OpenMP 4 threads.
 * Offload calculations to GPU accelerator device.
 *
 * This program will numerically compute the integral:
                   ^1
                  /  4 dx /( 1+ x*x )
        0v
  (from 0 to 1).  The value of this integral is Pi -- which
  is great since it gives us an easy way to check the answer.

 History: P. Gronek, 04/2018, 05/2020, 03/2023

  $ source /opt/nfs/config/source_clang170.sh
  $ source /opt/nfs/config/source_cuda117.sh
  $ clang -O2 -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda --cuda-path=/usr/local/cuda-11.7 lab1_pi_gpu.c -o lab1_pi_gpu
  $ nvprof ./lab1_pi_gpu 1024

 *
 **/
#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
double step;

int main(int argc, char **argv)
{
    long maxthreads, nteams, tsize, bsize, bleft, num_blocks;
    double x, pi, sum = 0.0;
    double start_time, run_time;

    // Host CPU code
    //
    if (argc > 1)
        num_steps = atol(argv[1]);
    start_time = omp_get_wtime();

    sum = 0.0;                      // sum of all rectangle heights
    step = 1.0 / (double)num_steps; // rectangle width

// Accelerator based code - region 1
//
// Use target directive to offload a region to device (GPU).
// Host and device share memory via mapping: to, from, tofrom.
#pragma omp target map(from : maxthreads)
#pragma omp master
    maxthreads = omp_get_max_threads(); // Executed on accelerator device.

    // Host CPU code
    //
    // Calculate thread blocks
    // For Turing TU106 GPU: 36 Multiprocessors, 64 CUDA Cores/MP
    // The FP64 TFLOP rate is 1/32nd the TFLOP rate of FP32 [|:-(]
    bsize = maxthreads * 16; // GPU thread block size (multiplied to fit MP)
    bleft = (num_steps % bsize) ? 1 : 0;
    num_blocks = num_steps / bsize + bleft;

// Accelerator based code - region 2
//
// Use target directive to offload a region to device (GPU).
// Host and device share memory via mapping: to, from, tofrom.
#pragma omp target map(to : step), map(tofrom : sum), map(from : nteams, tsize)
// Use teams directive to create multiple team leader threads
// that can execute in parallel on multiple processors on device.
// Also use distribute directive to spread iterations
// of a parallel loop across team leaders.
#pragma omp teams distribute num_teams(num_blocks) thread_limit(bsize) reduction(+ : sum)
    for (int i = 0; i < num_steps; i += bsize)
    {
// Only team leader threads active - workshare (without barrier)
// Use parallel and for directives to spread iteration in a team.
#pragma omp parallel for private(x) reduction(+ : sum)
        for (int j = i; j < i + bsize; j++)
        {
            // All team member threads active - workshare (with barrier)
            if (j < num_steps)
            { // Avoid exceeding iteration size limit.
                x = (j + 0.5) * step;
                sum = sum + 4.0 / (1.0 + x * x);
                if (j == 0) // Execute on one thread only.
                {
                    nteams = omp_get_num_teams();
                    tsize = omp_get_team_size(omp_get_level());
                }
            }
        } // End of parallel and for blocks.
    }     // End of distrbute, teams and target blocks.

    // Host CPU code
    //
    // Calculate Pi
    pi = step * sum;

    printf("\n Available OpenMP threads: %ld, Team size: %ld, No. of teams: %ld\n"
           " No. of steps: %ld, Oversubscribed threads: %ld\n Pi: %.16f \n",
           maxthreads, tsize, nteams, num_steps,
           ((num_steps / tsize + 1) * tsize - num_steps) % tsize,
           pi);

    run_time = omp_get_wtime() - start_time;
    printf("\n OpenMP runtime: %f seconds \n", run_time);

    return 0;
}
