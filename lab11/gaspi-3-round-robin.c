/*
   Communication Example

   Round robin communicaiton with
    gaspi_write_notify and gaspi_waitsome:

    - init local buffer
    - write to remote buffer
    - wait for data availability
    - print result
*/
/**
  *      GASPI round-robin.c
  *      http://www.gaspi.de/tutorial/
  *      modified by PG 2018, 2022
  *
  *** Setup:
 $ source /opt/nfs/config/source_gaspi.sh

  *** Compile:
 $ mpicc gaspi-3-round-robin.c -o gaspi-3-round-robin -I${GPI_INC_DIR} -L${GPI_LIB_DIR} -lGPI2 -lpthread

  *** Execute (gaspi_wrapper script is a workaround for GPI 1.5+ environment bug):
 $ gaspi_run -m nodes -n 4 $GPI_BIN_DIR/gaspi_wrapper.sh $(pwd)/gaspi-3-round-robin

  *** Output:
Startup time: 0 sec
Rank 0 initialized element 0: 0.000000
Rank 0 initialized element 1: 1.000000
Rank 0 initialized element 2: 2.000000
Rank 0 initialized element 3: 3.000000
Rank 0 received element 0: 12.000000
Rank 0 received element 1: 13.000000
Rank 0 received element 2: 14.000000
Rank 0 received element 3: 15.000000
Rank 1 initialized element 0: 4.000000
Rank 1 initialized element 1: 5.000000
Rank 1 initialized element 2: 6.000000
Rank 1 initialized element 3: 7.000000
Rank 1 received element 0: 0.000000
Rank 1 received element 1: 1.000000
Rank 1 received element 2: 2.000000
Rank 1 received element 3: 3.000000
Rank 2 initialized element 0: 8.000000
Rank 2 initialized element 1: 9.000000
Rank 2 initialized element 2: 10.000000
Rank 2 initialized element 3: 11.000000
Rank 2 received element 0: 4.000000
Rank 2 received element 1: 5.000000
Rank 2 received element 2: 6.000000
Rank 2 received element 3: 7.000000
Rank 3 initialized element 0: 12.000000
Rank 3 initialized element 1: 13.000000
Rank 3 initialized element 2: 14.000000
Rank 3 initialized element 3: 15.000000
Rank 3 received element 0: 8.000000
Rank 3 received element 1: 9.000000
Rank 3 received element 2: 10.000000
Rank 3 received element 3: 11.000000

  *
  **/
#include <GASPI.h>
#include <stdlib.h>

/* ************* HELPER MACROS ************* */

#ifndef ASSERT_H
#define ASSERT_H

#include <stdio.h>

#define ASSERT(x...)                                                      \
    if (!(x))                                                             \
    {                                                                     \
        fprintf(stderr, "Error: '%s' [%s:%i]\n", #x, __FILE__, __LINE__); \
        exit(EXIT_FAILURE);                                               \
    }

#endif

/* *** */

#ifndef SUCCESS_OR_DIE_H
#define SUCCESS_OR_DIE_H

#define SUCCESS_OR_DIE(f...)                                                \
    do                                                                      \
    {                                                                       \
        const gaspi_return_t r = f;                                         \
        if (r != GASPI_SUCCESS)                                             \
        {                                                                   \
            printf("Error: '%s' [%s:%i]: %i\n", #f, __FILE__, __LINE__, r); \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)
#endif

// Find neighbour processes in a round-robin cycle configuration
#define RIGHT(iProc, nProc) ((iProc + nProc + 1) % nProc)
#define LEFT(iProc, nProc) ((iProc + nProc - 1) % nProc)

/******************** MAIN PROGRAM ********************/

int main(int argc, char *argv[])
{
    int j;
    static const int VLEN = 1 << 2; // number of data elements = 4

    // Initialise communication - blocking call form
    SUCCESS_OR_DIE(gaspi_proc_init(GASPI_BLOCK));

    // Get the calling process number and total number of processes running
    gaspi_rank_t iProc, nProc;
    SUCCESS_OR_DIE(gaspi_proc_rank(&iProc));
    SUCCESS_OR_DIE(gaspi_proc_num(&nProc));

    // Prepare size of data segment (half for exchange with each neighbour)
    gaspi_segment_id_t const segment_id = 0;
    gaspi_size_t const segment_size = 2 * VLEN * sizeof(double);

    // Allocate shared segment in local process memory, make it available remotely
    SUCCESS_OR_DIE(gaspi_segment_create(segment_id,             // set shared segment id
                                        segment_size,           // set segment size
                                        GASPI_GROUP_ALL,        // access for all processes
                                        GASPI_BLOCK,            // blocking call form
                                        GASPI_MEM_UNINITIALIZED // content not initialized
                                        ));

    // Set pointer to the shared segment starting position
    gaspi_pointer_t array;
    SUCCESS_OR_DIE(gaspi_segment_ptr(segment_id,
                                     &array));
    // Set local pointers to the lower and upper half of shared segment
    double *src_array = (double *)(array); // data to be sent
    double *rcv_array = src_array + VLEN;  // data to be received

    // Initialise content of the lower half of shared segment
    for (j = 0; j < VLEN; ++j)
        src_array[j] = (double)(iProc * VLEN + j);

    // Print initial data to be send to right neighbour (lower half of local segment)
    for (j = 0; j < VLEN; ++j)
    {
        printf("Rank %d initialized element %d: %f \n",
               iProc,
               j,
               src_array[j]);
    }

    // Set notification instance id (for data sending)
    gaspi_notification_id_t data_available = 0;

    // Set communication queue id
    gaspi_queue_id_t queue_id = 0;

    // Set offsets for the lower and upper half of shared segment
    gaspi_offset_t loc_off = 0;                     // to be used with
    gaspi_offset_t rem_off = VLEN * sizeof(double); // data transfers

    // Wait for local completion of all requests in a given queue
    SUCCESS_OR_DIE(gaspi_wait(queue_id,      // communication queue
                              GASPI_BLOCK)); // blocking call form

    // Put data by one-sided write to right neighbour's segment and notify it
    SUCCESS_OR_DIE(gaspi_write_notify(segment_id,            // local shared segment id
                                      loc_off,               // local position offset
                                      RIGHT(iProc, nProc),   // destination process
                                      segment_id,            // remote shared segment id
                                      rem_off,               // remote position offset
                                      VLEN * sizeof(double), // size of data transferred
                                      data_available,        // remote notification id
                                      1 + iProc,             // notification value sent (> 0)
                                      queue_id,              // communication queue used
                                      GASPI_BLOCK            // blocking call form
                                      ));

    // Set notification instance (for data reception)
    gaspi_notification_id_t id;
    // Set expected value of notification from left neighbour
    gaspi_notification_t expected = 1 + LEFT(iProc, nProc);

    // Wait for notifications (of non-0 value)  from a neighbour
    SUCCESS_OR_DIE(gaspi_notify_waitsome(segment_id,     // related shared segment id
                                         data_available, // starting id for incoming notifications
                                         1,              // number of notifications to wait for
                                         &id,            // first received notification id
                                         GASPI_BLOCK));  // blocking call form

    // Verify if received notification id is an expected one
    ASSERT(id == data_available);

    // Retrieve notification value and reset it to zero atomically
    gaspi_notification_t value;
    SUCCESS_OR_DIE(gaspi_notify_reset(segment_id, // related shared segment id
                                      id,         // notification instance to be reset
                                      &value));   // previous value (before the reset) retrieved

    // Verify if received notification value is an expected one (meaning related data transfer is completed)
    ASSERT(value == expected);

    // Print data received from left neighbour (upper half of local segment)
    for (j = 0; j < VLEN; ++j)
    {
        printf("Rank %d received element %d: %f \n",
               iProc,
               j,
               rcv_array[j]);
    }

    // Wait for local completion of all requests in a given queue
    SUCCESS_OR_DIE(gaspi_wait(queue_id,      // communication queue used
                              GASPI_BLOCK)); // blocking call form

    // Disconnect from other processes
    SUCCESS_OR_DIE(gaspi_proc_term(GASPI_BLOCK));

    return EXIT_SUCCESS;
}
