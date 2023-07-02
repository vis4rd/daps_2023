/****************************************************************
 * 2D stencil code using MPI and OpenMP (GPU device offloading).
 *
 * 2D regular grid is divided into px * py blocks of grid points
 * (px * py = # of processes.)
 * In every iteration, each process calls nonblocking operations
 * with derived data types to exchange grid points in a halo
 * with its neighbors.
 * Computation is accelerated by OpenMP offloading features.
 *
 * 05/2023 - D-10 Lab.204

 *** Configure (MPICH 4.1.1, Clang 17.0, CUDA 11.7):

  source /opt/nfs/config/source_mpich411.sh
  source /opt/nfs/config/source_clang170.sh
  source /opt/nfs/config/source_cuda.sh

  /opt/nfs/config/station204_name_list.sh 1 16 > nodes

 *** Compile:

  clang -fopenmp -fopenmp-targets=nvptx64-nvidia-cuda \
                     --cuda-path=/usr/local/cuda-11.7 \
        lab_stencil2d_mpi_omp4_gpu.c -o lab_stencil2d_mpi_omp4_gpu \
        -I/opt/nfs/mpich-4.1.1/include -L/opt/nfs/mpich-4.1.1/lib  \
        -Wl,-rpath -Wl,/opt/nfs/mpich-4.1.1/lib -Wl,--enable-new-dtags \
        -lmpi -lm

 *** Run (under nVidia CUDA profiler):

  mpiexec -l -n 4 -f nodes /usr/local/cuda/bin/nvprof \
      ./lab_stencil2d_mpi_omp4_gpu 100 10 1000 2 2 |& sort -g -k1.1

 *** or (for a full cluster):

 /opt/nfs/config/station204_name_list.sh 1 16 |& tee nodes && for i in $(( $(cat nodes | wc -l) )) ; do mpiexec -l -n $(( ($i/2) * 2 )) -f nodes ./lab_stencil2d_mpi_omp4_gpu $(( ($i/2) * 20 * 4 )) 10 1000 2 $(( $i/2 )) |& sort -g -k1.1 ; done
 eom output-*.bmp

 ***/

#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/******************************************************/

/* row-major order */
#define ind(i, j) ((j) * (bx + 2) + (i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr);

void printarr_par(int iter, double *array, int size, int px, int py, int rx, int ry, int bx, int by,
                  int offx, int offy, int (*ind)(int i, int j, int bx), MPI_Comm comm);

/******************************************************/

int main(int argc, char **argv)
{
    int rank, size, provided;
    int n, energy, niters, px, py;

    int rx, ry;
    int north, south, west, east;
    int bx, by, offx, offy;

    const int nsources = 3; /* three heat sources */
    int sources[nsources][2];
    int locnsources;             /* number of sources in my area */
    int locsources[nsources][2]; /* sources local to my rank */

    double t1, t2;

    int iter, i;

    double *aold, *anew, *tmp;

    double heat, rheat;

    int final_flag;

    /* initialize MPI envrionment with multithreading support
     * - needed for cooperation with OpenMP multihreaded processing
     * - MPI ops will be called by master thread only (funneled)
     **/
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED)
        MPI_Abort(MPI_COMM_WORLD, 1);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* argument checking and setting */
    setup(rank, size, argc, argv, &n, &energy, &niters, &px, &py, &final_flag);

    if (final_flag == 1)
    {
        MPI_Finalize();
        exit(0);
    }

    /* determine my coordinates (x,y) -- rank=x*a+y in the 2d processor array */
    rx = rank % px;
    ry = rank / px;

    /* determine my four neighbors */
    north = (ry - 1) * px + rx;
    if (ry - 1 < 0)
        north = MPI_PROC_NULL;
    south = (ry + 1) * px + rx;
    if (ry + 1 >= py)
        south = MPI_PROC_NULL;
    west = ry * px + rx - 1;
    if (rx - 1 < 0)
        west = MPI_PROC_NULL;
    east = ry * px + rx + 1;
    if (rx + 1 >= px)
        east = MPI_PROC_NULL;

    /* decompose the domain */
    bx = n / px;    /* block size in x */
    by = n / py;    /* block size in y */
    offx = rx * bx; /* offset in x */
    offy = ry * by; /* offset in y */

    /* printf("%i (%i,%i) - w: %i, e: %i, n: %i, s: %i\n", rank, ry,rx,west,east,north,south); */

    /* allocate working arrays & communication buffers */
    aold = (double *)malloc((bx + 2) * (by + 2) * sizeof(double)); /* 1-wide halo zones! */
    anew = (double *)malloc((bx + 2) * (by + 2) * sizeof(double)); /* 1-wide halo zones! */

    memset(aold, 0, (bx + 2) * (by + 2) * sizeof(double));
    memset(anew, 0, (bx + 2) * (by + 2) * sizeof(double));

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    /* create east-west datatype */
    MPI_Datatype east_west_type;
    MPI_Type_vector(by, 1, bx + 2, MPI_DOUBLE, &east_west_type);
    MPI_Type_commit(&east_west_type);

    t1 = MPI_Wtime(); /* take time */

    /* create GPU device data offloading domain */
#pragma omp target data map(to : aold[0 : (bx + 2) * (by + 2)], anew[0 : (bx + 2) * (by + 2)])
    for (iter = 0; iter < niters; ++iter)
    {

        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i)
        {
            aold[ind(locsources[i][0], locsources[i][1])] += energy; /* heat source */
        }

        /* exchange data with neighbors */
        MPI_Request reqs[8];
        MPI_Isend(&aold[ind(1, 1)] /* north */, bx, MPI_DOUBLE, north, 9, MPI_COMM_WORLD,
                  &reqs[0]);
        MPI_Isend(&aold[ind(1, by)] /* south */, bx, MPI_DOUBLE, south, 9, MPI_COMM_WORLD,
                  &reqs[1]);
        MPI_Isend(&aold[ind(bx, 1)] /* east */, 1, east_west_type, east, 9, MPI_COMM_WORLD,
                  &reqs[2]);
        MPI_Isend(&aold[ind(1, 1)] /* west */, 1, east_west_type, west, 9, MPI_COMM_WORLD,
                  &reqs[3]);
        MPI_Irecv(&aold[ind(1, 0)] /* north */, bx, MPI_DOUBLE, north, 9, MPI_COMM_WORLD,
                  &reqs[4]);
        MPI_Irecv(&aold[ind(1, by + 1)] /* south */, bx, MPI_DOUBLE, south, 9,
                  MPI_COMM_WORLD, &reqs[5]);
        MPI_Irecv(&aold[ind(bx + 1, 1)] /* west */, 1, east_west_type, east, 9, MPI_COMM_WORLD,
                  &reqs[6]);
        MPI_Irecv(&aold[ind(0, 1)] /* east */, 1, east_west_type, west, 9, MPI_COMM_WORLD,
                  &reqs[7]);
        MPI_Waitall(8, reqs, MPI_STATUSES_IGNORE);

        /* update data in GPU device memory */
#pragma omp target update to(aold[0 : (bx + 2) * (by + 2)])

        /* offload computation to the GPU device in update_grid() */
        /* update grid points */
        update_grid(bx, by, aold, anew, &heat);

        /* update data in CPU host memory */
#pragma omp target update from(anew[0 : (bx + 2) * (by + 2)])

        /* swap working arrays */
        tmp = anew;
        anew = aold;
        aold = tmp;

        /* optional - print image */
        if (iter == niters - 1)
            printarr_par(iter, anew, n, px, py, rx, ry, bx, by, offx, offy, ind_f, MPI_COMM_WORLD);
    } /* GPU device data offloading domain ends here */

    t2 = MPI_Wtime();

    /* free working arrays and communication buffers */
    free(aold);
    free(anew);

    MPI_Type_free(&east_west_type);

    /* get final heat in the system */
    MPI_Allreduce(&heat, &rheat, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (!rank)
        printf("[%i] last heat: %f time: %f\n", rank, rheat, t2 - t1);

    MPI_Finalize();
    return 0;
}

/******************************************************/

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag)
{
    int n, energy, niters, px, py;

    (*final_flag) = 0;

    if (argc < 6)
    {
        if (!rank)
            printf("usage: stencil_mpi <n> <energy> <niters> <px> <py>\n");
        (*final_flag) = 1;
        return;
    }

    n = atoi(argv[1]);      /* nxn grid */
    energy = atoi(argv[2]); /* energy to be injected per iteration */
    niters = atoi(argv[3]); /* number of iterations */
    px = atoi(argv[4]);     /* 1st dim processes */
    py = atoi(argv[5]);     /* 2nd dim processes */

    if (px * py != proc)
        MPI_Abort(MPI_COMM_WORLD, 1); /* abort if px or py are wrong */
    if (n % py != 0)
        MPI_Abort(MPI_COMM_WORLD, 2); /* abort px needs to divide n */
    if (n % px != 0)
        MPI_Abort(MPI_COMM_WORLD, 3); /* abort py needs to divide n */

    (*n_ptr) = n;
    (*energy_ptr) = energy;
    (*niters_ptr) = niters;
    (*px_ptr) = px;
    (*py_ptr) = py;
}

/******************************************************/

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2])
{
    int i, locnsources = 0;

    sources[0][0] = n / 2;
    sources[0][1] = n / 2;
    sources[1][0] = n / 3;
    sources[1][1] = n / 3;
    sources[2][0] = n * 4 / 5;
    sources[2][1] = n * 8 / 9;

    for (i = 0; i < nsources; ++i)
    { /* determine which sources are in my patch */
        int locx = sources[i][0] - offx;
        int locy = sources[i][1] - offy;
        if (locx >= 0 && locx < bx && locy >= 0 && locy < by)
        {
            locsources[locnsources][0] = locx + 1; /* offset by halo zone */
            locsources[locnsources][1] = locy + 1; /* offset by halo zone */
            locnsources++;
        }
    }

    (*locnsources_ptr) = locnsources;
}

/**************************************************
 *
 * Update grid by offloading stencil calculations
 * to GPU accelerator using OpenMP 4.x directives
 *
 */
void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr)
{
    int i, j;
    double heat = 0.0;

#pragma omp target teams distribute parallel for simd collapse(2) firstprivate(bx, by) \
    reduction(+ : heat)
    for (i = 1; i < bx + 1; ++i)
    {
        for (j = 1; j < by + 1; ++j)
        {
            anew[ind(i, j)] =
                anew[ind(i, j)] / 2.0 + (aold[ind(i - 1, j)] + aold[ind(i + 1, j)] +
                                         aold[ind(i, j - 1)] + aold[ind(i, j + 1)]) /
                                            4.0 / 2.0;
            heat += anew[ind(i, j)];
        }
    }

    (*heat_ptr) = heat;
}

/******************************************************/

struct bmpfile_magic
{
    unsigned char magic[2];
};

struct bmpfile_header
{
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

struct bmpinfo_header
{
    uint32_t header_sz;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
};

/**************************************************
 *
 * Print calculation results to file using parallel
 * MPI I/O ops from all process in sequential mode
 *
 */
void printarr_par(int iter, double *array, int size, int px, int py, int rx, int ry, int bx, int by,
                  int offx, int offy, int (*index)(int, int, int), MPI_Comm comm)
{

    int myrank;

    int xcnt, ycnt, my_xcnt, my_ycnt;
    int i;
    int linesize = bx * 3;
    int padding;
    char *myline;

    MPI_File fh;
    char fname[128];
    snprintf(fname, 128, "./output-%i.bmp", iter);

    MPI_File_open(comm, fname, MPI_MODE_SEQUENTIAL | MPI_MODE_CREATE | MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &fh);

    MPI_Comm_rank(comm, &myrank);

    if (myrank == 0)
    {

        struct bmpfile_magic magic;
        struct bmpfile_header header;
        struct bmpinfo_header bmpinfo;

        magic.magic[0] = 0x42;
        magic.magic[1] = 0x4D;

        MPI_File_write_shared(fh, &magic, sizeof(struct bmpfile_magic), MPI_BYTE,
                              MPI_STATUSES_IGNORE);

        header.filesz =
            sizeof(struct bmpfile_magic) + sizeof(struct bmpfile_header) +
            sizeof(struct bmpinfo_header) + size * (size * 3 + size * 3 % 4);
        header.creator1 = 0xFE;
        header.creator1 = 0xFE;
        header.bmp_offset =
            sizeof(struct bmpfile_magic) + sizeof(struct bmpfile_header) +
            sizeof(struct bmpinfo_header);

        MPI_File_write_shared(fh, &header, sizeof(struct bmpfile_header), MPI_BYTE,
                              MPI_STATUSES_IGNORE);

        bmpinfo.header_sz = sizeof(struct bmpinfo_header);
        bmpinfo.width = size;
        bmpinfo.height = size;
        bmpinfo.nplanes = 1;
        bmpinfo.bitspp = 24;
        bmpinfo.compress_type = 0;
        bmpinfo.bmp_bytesz = size * (size * 3 + size * 3 % 4);
        bmpinfo.hres = size;
        bmpinfo.vres = size;
        bmpinfo.ncolors = 0;
        bmpinfo.nimpcolors = 0;

        MPI_File_write_shared(fh, &bmpinfo, sizeof(struct bmpinfo_header), MPI_BYTE,
                              MPI_STATUSES_IGNORE);
    }

    padding = 0;
    if (((rx + 1) % px) == 0)
        padding = size * 3 % 4;
    myline = (char *)malloc(linesize + padding);

    my_xcnt = 0;
    my_ycnt = 0;
    xcnt = 0;
    ycnt = size;

    while (ycnt >= 0)
    {
        MPI_Barrier(comm);
        if ((xcnt == offx) && (ycnt >= offy) && (ycnt < offy + by))
        {
            for (i = 0; i < linesize; i += 3)
            {
                int rgb;
                if (i / 3 > bx)
                    rgb = 0;
                else
                    rgb = (int)round(255.0 * array[index(i / 3, by - my_ycnt, bx)]);
                if ((i == 0) || (i / 3 == bx - 1) || (my_ycnt == 0) || (my_ycnt == by - 1))
                    rgb = 255;
                if (rgb > 255)
                    rgb = 255;
                myline[i + 0] = 0;
                myline[i + 1] = 0;
                myline[i + 2] = rgb;
            }
            my_xcnt += bx;
            my_ycnt++;
            MPI_File_write_shared(fh, myline, linesize + padding, MPI_BYTE, MPI_STATUSES_IGNORE);
        }
        xcnt += bx;
        if (xcnt >= size)
        {
            xcnt = 0;
            ycnt--;
        }
    }

    free(myline);

    MPI_File_close(&fh);
}

/******************************************************/
