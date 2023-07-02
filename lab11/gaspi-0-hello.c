/**
  *      GASPI hello.c
  *      http://www.gaspi.de/tutorial/
  *      modified by PG 2018, 2022
  *
  *** Setup:
 $ source /opt/nfs/config/source_gaspi.sh

  *** Compile:
 $ mpicc gaspi-0-hello.c -o gaspi-0-hello -I${GPI_INC_DIR} -L${GPI_LIB_DIR} -lGPI2 -lpthread

  *** Execute (gaspi_wrapper script is a workaround for GPI 1.5+ environment bug):
 $ gaspi_run -m nodes -n 4 $GPI_BIN_DIR/gaspi_wrapper.sh $(pwd)/gaspi-0-hello

  *** Output:
Startup time: 0 sec
Hello from rank 0 of 4
Hello from rank 1 of 4
Hello from rank 2 of 4
Hello from rank 3 of 4

  *
  **/
#include <stdlib.h>
#include <stdio.h>
/* GPI-2 (GASPI) header */
#include <GASPI.h>

/* ************* HELPER MACROS ************* */

void success_or_exit(const char *file, const int line, const int ec)
{
    if (ec != GASPI_SUCCESS)
    {
        printf("Assertion failed in %s[%i]:%d\n", file, line, ec);
        exit(EXIT_FAILURE);
    }
}

#define ASSERT(ec) success_or_exit(__FILE__, __LINE__, ec);

/******************** MAIN PROGRAM ********************/

int main(int argc, char *argv[])
{
    gaspi_rank_t rank, num;
    gaspi_return_t ret;

    /* Initialize GPI-2 */
    // Initialise communication - blocking call form
    ASSERT(gaspi_proc_init(GASPI_BLOCK));

    /* Get ranks information */
    // Get the call process number and total number of processes running
    ASSERT(gaspi_proc_rank(&rank));
    ASSERT(gaspi_proc_num(&num));

    /* creation of global segment for communication */
    // Allocate shared segment in local process memory, make it available remotely
    gaspi_segment_create(0,                      // set shared segment id
                         (1 << 31),              // set segment size = 2GB
                         GASPI_GROUP_ALL,        // allow access for group members
                         GASPI_BLOCK,            // blocking call form
                         GASPI_MEM_INITIALIZED); // content not initialized

    printf("Hello from rank %d of %d\n", rank, num);

    /* Terminate */
    // Disconnect from other processes
    ASSERT(gaspi_proc_term(GASPI_BLOCK));

    return EXIT_SUCCESS;
}
