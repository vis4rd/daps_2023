/* _test_app_matmult.c: matrix multiplication program.

    source /opt/nfs/config/source_bupc_2022.10.sh
    upcc -Wc,"-O2" -bupc -network=udp -pthreads=4 lab10/upc-1-matmul.c

        Description:
    - Matrix multiplication program. This program performs the following matrix
      multiplication operation. User can change the data type by simply changing
      the DATA_T macros.
        a[ROW x colA] . b[colA x colB] = c[ROW x colB]
    - Matrix a is divided up among the threads so that row 0 has affinity to
      thread 0, and row i has affinity to thread i.
    - Matrix b is divided up among the threads so that column 0 has affinity to
      thread 0 and column i has affinity to thread i.
    - Matrix c is divided in the same fashion as matrix a.

*/

#include <upc.h>
#include <stdio.h>

// #define VERBOSE0  // enable printing of input matices

//// Set matrix data type
#define DTYPE int

//// Set number of rows for a[][] and c[][]
#define ROW (THREADS * 2)

//// Set number of columns
/**  PG Taurus 25/05/2016:
  *  bug fix - in the UPC dynamic translation environment,
  *  THREADS must appear exactly once in declarations of shared arrays
#define colA (THREADS*2)
#define colB (THREADS*2)
  ***/
#define SIZE 16 // fixed size
#define colA SIZE
#define colB SIZE
/** THREADS is used in shared matrices declarations **/
shared[colA] DTYPE a[ROW][colA];
shared[colB] DTYPE c[ROW][colB];
/**  PG PG Taurus 25/05/2016:
  *  bug fix - see note above
shared DTYPE b[colA][colB];
  ***/
shared DTYPE b[colA][ROW];

/////////////////////////////////////////////////////////////////////

int main()
{
    int i, j, k;
    DTYPE sum;

    //// Initialize a[][] anb b[][] matrices
    upc_forall(i = 0; i < ROW; i++; &a[i][0])
    {
        for (j = 0; j < colA; j++)
        {
            a[i][j] = (DTYPE)(i) + (DTYPE)(j) + (DTYPE)(1);
        }
    }

    for (i = 0; i < colA; i++)
    {
        upc_forall(j = 0; j < colB; j++; &b[i][j])
        {
            b[j][i] = (DTYPE)(j) - (DTYPE)(i) + (DTYPE)(225 / (j + 1));
        }
    }

    upc_barrier;

////  Print a[][] and b[][] - only if VERBOSE0 is set
#ifdef VERBOSE0
    if (MYTHREAD == 0)
    {
        for (i = 0; i < ROW; i++)
        {
            printf("\n\n");
            for (j = 0; j < colA; j++)
                printf("(%d)a[%d][%d] = %d   ", (int)upc_threadof(&a[i][j]), i, j, a[i][j]);
        }
        printf("\n\n");

        for (i = 0; i < colA; i++)
        {
            printf("\n\n");
            for (j = 0; j < colB; j++)
                printf("(af=%d)b[%d][%d] = %d   ", (int)upc_threadof(&b[i][j]), i, j, b[i][j]);
        }
        printf("\n\n");
    }
#endif
    //// Calculate matix product c[][]
    upc_forall(i = 0; i < ROW; i++; &a[i][0])
    {
        for (j = 0; j < colB; j++)
        {
            sum = (DTYPE)(0);
            for (k = 0; k < colA; k++)
                sum += a[i][k] * b[k][j];
            c[i][j] = sum;
        }
    }

    upc_barrier;

    // Print c[][] matix - rows by thread
    if (MYTHREAD == 0)
    {
        for (i = 0; i < ROW; i++)
        {
            printf("\n");
            for (j = 0; j < colB; j++)
            {
                if ((i * SIZE + j) % 4 == 0)
                    printf("\nThread\t%d: ", (int)upc_threadof(&c[i][j]));
                printf(" c[%d][%d] = %d \t", i, j, c[i][j]);
            }
        }
        /* DOB: TODO, add a real result correctness check */
        printf("\nSuccess: application did not crash\n");
    }

    return 0;
}
