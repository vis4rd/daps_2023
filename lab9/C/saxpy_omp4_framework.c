/*
 OpenMP 4.x program using Nvidia GPGPU device offloading

 Configuration and compilation:
  cat /opt/nfs/config/README.OpenMP4

 Execution (under Nvidia profiler - optional):
  nvprof ./example_omp4_gpu 1024

*/

/*
     SAXPY: Single precision Y = (A * X) Plus Y

     THIS IS A FRAMEWORK ONLY - fix all the FIXME-s !!!
*/

#define FIXME 100 // This is incorrect !!! Get rid of all FIXME in the code!

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, const char *argv[])
{
    // main thread only

    if (argc != 2)
    {
        printf("Usage: %s A_matriX_size\n", argv[0]);
        return 0;
    }

    int n = atoi(argv[1]);
    // allocate x[], y[]
    double *x = (double *)malloc(sizeof(double) * n);
    double *y = (double *)malloc(sizeof(double) * n);
    // initialize x[], y[]
    double idrandmax = 1.0 / RAND_MAX;
    double a = 1.0; // idrandmax * rand();
    for (int i = 0; i < n; i++)
    {
        x[i] = 1.0; // idrandmax * rand();
        y[i] = 1.0; // idrandmax * rand();
    }
    // calculate thread blocks
    /*
     STEP 1: CALCULATE DISTRIBUTION OF DEVICE THREADS INTO BLOCKS
    */
    int bsize = FIXME;          // assume number of threads in a block
    int bleft = FIXME;          // blocks left ...?
    int num_blocks = n / FIXME; // number of blocks needed
    printf("num_blocks=%d \n", num_blocks);

    // parallel code

    int ncprocs;
#pragma omp parallel
#pragma omp master
    ncprocs = omp_get_num_procs();
    printf("\nnumber of cpu procs=%d\n\n", ncprocs);

// map x[] to accelerator memory (one way)
#pragma omp target data map(to : x[0 : n])
    {

        int ntprocs;
// target - map ntprocs from target to host
#pragma omp target map(from : ntprocs)
#pragma omp master
        ntprocs = omp_get_num_procs(); // executes on device

        // print on host
        printf("number of target procs=%d\n", ntprocs);

        printf("\non target:\n\n");

// target - map y[] to and from accelerator memory (two ways)
#pragma omp target map(tofrom : y[0 : n])
// create thread teams (only leader thread active in each team)
#pragma omp teams num_teams(num_blocks) thread_limit(bsize)
// now only leader threads execute - all the same code
//
// distribute loop iterations between teams
#pragma omp distribute
        /*
          STEP 2: DISTRIBUTE OUTER LOOP ITERATIONS AMONG TEAMS OF DEVICE THREADS
        */
        for (int i = 0; i < n; i += FIXME) // what index step should be ...?
        {
// leader threads - workshare (without barrier)
#pragma omp parallel for
            /*
              STEP 3: DISTRIBUTE INNER LOOP ITERATIONS AMONG THREADS OF A SINGLE TEAM
            */
            for (int j = FIXME; j < FIXME; j++) // what index range should be ...?
            {
                // all team member threads active - workshare (with barrier)
                /*
                  STEP 4: PERFORM CALCULATIONS ON ALL DATA ...
                */
                if (j < FIXME) // why ...?
                {
                    y[j] += a * x[j]; // fully parallelized code
                    printf("iteration j= %d /%d by team=%d/%d thread=%d/%d y[]=%f\n",
                           j, n, omp_get_team_num(), omp_get_num_teams(),
                           omp_get_thread_num(), omp_get_team_size(omp_get_level()),
                           y[j]);
                }
            } // end of 'parallel for' region
        }     // common end of 'target', 'teams' and 'distribute' regions

        // y[] modified on the host here
        int m = 0;
        printf("\non host - parallel code ");
#pragma omp parallel for
        for (int i = 0; i < n; i++)
        {
            /*
              STEP 5: VERIFY DATA TRANSFER - AND CHANGE DATA ON HOST USING CPU THREADS
            */
#pragma omp FIXME // what directive to use here and why ...?
            {
                if (m == 0)
                    printf("- with %d cpu threads:\n\n",
                           omp_get_num_threads());
                m = 1;
            }
            printf("y[%d]= %f, ", i, y[i]);
            y[i] = 0.0;
        }
        printf("\n\non target:\n\n");

        /* REPEAT DEVICE CALCULATIONS - WITH MODIFIED DATA ...
           (fix FIXME-s the same way as above)  */

// target -  map y[] to and from accelerator memory (two ways)
#pragma omp target map(tofrom : y[0 : n])
// create thread teams (only leader thread active in each team)
#pragma omp teams num_teams(num_blocks) thread_limit(bsize)
// now only leader threads execute - all the same code
//
// distribute loop iterations between teams
#pragma omp distribute
        /*
          STEP 2a: DISTRIBUTE OUTER LOOP ITERATIONS AMONG TEAMS OF DEVICE THREADS
        */
        for (int i = 0; i < n; i += FIXME)
        {
// leader threads - workshare (without barrier)
#pragma omp parallel for
            /*
              STEP 3a: DISTRIBUTE INNER LOOP ITERATIONS AMONG THREADS OF A SINGLE TEAM
            */
            for (int j = FIXME; j < FIXME; j++)
            {
                // all team member threads active - workshare (with barrier)
                /*
                  STEP 4a: PERFORM CALCULATIONS ON ALL DATA ...
                */
                if (j < FIXME)
                {
                    y[j] += a * x[j]; // fully parallelized code
                    printf("iteration j= %d /%d by team=%d/%d thread=%d/%d y[]=%f\n",
                           j, n, omp_get_team_num(), omp_get_num_teams(),
                           omp_get_thread_num(), omp_get_num_threads(),
                           y[j]);
                }
            } // end of 'parallel for' region
        }     // common end of 'target', 'teams' and 'distribute' regions

    } // end of 'target data' region

    // main CPU thread only

    double avg = 0.0, min = y[0], max = y[0];
    printf("\non host:\n\n");
    for (int i = 0; i < n; i++)
    {
        printf("y[%d]= %f, ", i, y[i]);
        avg += y[i];
        if (y[i] > max)
            max = y[i];
        if (y[i] < min)
            min = y[i];
    }

    printf("\n\nmin=%f, max=%f, avg=%f\n\n", min, max, avg / n);

    free(x);
    free(y);

    return 0;
}

/*
 =========================================================
 Results - program execution output should look like this:
 =========================================================

num_blocks=16

number of cpu procs=4

number of target procs=1024

on target:

(...)
iteration j= 0 /1024 by team=0/16 thread=0/64 y[]=2.000000
iteration j= 1 /1024 by team=0/16 thread=1/64 y[]=2.000000
iteration j= 2 /1024 by team=0/16 thread=2/64 y[]=2.000000
iteration j= 3 /1024 by team=0/16 thread=3/64 y[]=2.000000
iteration j= 4 /1024 by team=0/16 thread=4/64 y[]=2.000000
(...)
iteration j= 1020 /1024 by team=15/16 thread=60/64 y[]=2.000000
iteration j= 1021 /1024 by team=15/16 thread=61/64 y[]=2.000000
iteration j= 1022 /1024 by team=15/16 thread=62/64 y[]=2.000000
iteration j= 1023 /1024 by team=15/16 thread=63/64 y[]=2.000000

on host - with 4 cpu threads:

y[0]= 2.000000, y[512]= 2.000000, y[256]= 2.000000, y[768]= 2.000000,
(...)
y[764]= 2.000000, y[765]= 2.000000, y[766]= 2.000000, y[767]= 2.000000,

on target:

(...)
iteration j= 0 /1024 by team=0/16 thread=0/64 y[]=1.000000
iteration j= 1 /1024 by team=0/16 thread=1/64 y[]=1.000000
iteration j= 2 /1024 by team=0/16 thread=2/64 y[]=1.000000
iteration j= 3 /1024 by team=0/16 thread=3/64 y[]=1.000000
(...)
iteration j= 1019 /1024 by team=15/16 thread=59/64 y[]=1.000000
iteration j= 1020 /1024 by team=15/16 thread=60/64 y[]=1.000000
iteration j= 1021 /1024 by team=15/16 thread=61/64 y[]=1.000000
iteration j= 1022 /1024 by team=15/16 thread=62/64 y[]=1.000000
iteration j= 1023 /1024 by team=15/16 thread=63/64 y[]=1.000000

on host:

y[0]= 1.000000, y[1]= 1.000000, y[2]= 1.000000, y[3]= 1.000000,
(...)
y[1020]= 1.000000, y[1021]= 1.000000, y[1022]= 1.000000, y[1023]= 1.000000,

min=1.000000, max=1.000000, avg=1.000000

==22531== Profiling application: ./saxpy_omp4_lab4 1024
==22531== Profiling result:
Time(%)      Time     Calls       Avg       Min       Max  Name
 50.01%  2.7758ms         1  2.7758ms  2.7758ms  2.7758ms  __omptgt__2_1c41b04_21_
 48.93%  2.7158ms         1  2.7158ms  2.7158ms  2.7158ms  __omptgt__1_1c41b04_21_
  0.58%  32.417us        14  2.3150us  2.2080us  2.8480us  [CUDA memcpy DtoH]
  0.38%  21.344us        17  1.2550us  1.1520us  2.1440us  [CUDA memcpy HtoD]
  0.09%  4.8640us         1  4.8640us  4.8640us  4.8640us  __omptgt__0_1c41b04_21_

==22531== API calls:
Time(%)      Time     Calls       Avg       Min       Max  Name
 35.04%  62.296ms         1  62.296ms  62.296ms  62.296ms  cuModuleLoadDataEx
 31.31%  55.657ms         1  55.657ms  55.657ms  55.657ms  cuCtxCreate
 13.73%  24.406ms         1  24.406ms  24.406ms  24.406ms  cuCtxDestroy
 10.53%  18.721ms         1  18.721ms  18.721ms  18.721ms  cuModuleUnload
  8.01%  14.244ms        14  1.0174ms  30.610us  6.9123ms  cuMemcpyDtoH
  0.29%  516.04us        83  6.2170us     109ns  241.92us  cuDeviceGetAttribute
  0.29%  509.32us         1  509.32us  509.32us  509.32us  cudaGetDeviceProperties
  0.25%  440.57us         3  146.86us  15.533us  402.53us  cuLaunchKernel
  0.20%  357.45us        15  23.829us  4.0980us  132.03us  cuMemAlloc
  0.15%  263.89us        15  17.592us  4.3140us  103.49us  cuMemFree
  0.12%  205.68us        17  12.098us  3.9350us  58.579us  cuMemcpyHtoD
  0.03%  52.818us         3  17.606us  15.108us  22.354us  cuModuleGetFunction
  0.02%  39.618us         1  39.618us  39.618us  39.618us  cuDeviceTotalMem
  0.02%  37.966us         1  37.966us  37.966us  37.966us  cuDeviceGetName
  0.01%  11.155us        59     189ns     109ns     842ns  cuCtxSetCurrent
  0.00%  5.2410us         6     873ns     622ns  1.4790us  cuModuleGetGlobal
  0.00%  1.3130us         3     437ns     140ns     911ns  cuDeviceGetCount
  0.00%  1.1180us         3     372ns     226ns     594ns  cuDeviceGet
  0.00%     666ns         1     666ns     666ns     666ns  cuInit
  0.00%     447ns         1     447ns     447ns     447ns  cuDriverGetVersion

*/
