/** 
 * @file main.c
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "utils.h"
#include "ringbuffer.h"

#define X_MILLION 1 /* how many millions of message this program would sent by default*/

/* declarations of functions*/

/* this is a function parent process would execute, including: set up struct's pointers, 
  set up random number generator, while loop to send the messages and send the end 
  message, sum all sent numbers
  Return: void
  Params: void* : the pointer to the shared memory we get earlier
          unsigned int: how manay millions messages we want to send
*/
void do_parent_process(void* mem_ptr, unsigned int x_million);

/* this is a function child process would execute, including: get the shared
  memory, set up struct's pointers, loop to receive the messages and sum all
  received numbers
  Return: void
  Params: size_t: the size of the shared memory we need
*/
void do_child_process(size_t mem_size);

/* create shared memory. Parent process has a different behavior from child since
  they use difference shmget() flags 
  Return: void*: the pointer to the shared memory 
  Params: size_t mem_size: tells how much size we need for the shared memory
          int is_parent: distinguishes if the caller process is parent or not
          int* shm_id: pointer to shm_id, used to do shm controls later 
*/
void* create_global_common_area(size_t mem_size, int is_parent, int* shm_id);




/* main entrance of the application */
int main(int argc, char* argv[]) {
  /* parse args from the command line */
  unsigned int x_million = X_MILLION; // set it as the default value
  int parse_status = parse_args(argc, argv, &x_million);
  if ( parse_status < 0)
  {
    exit(EXIT_FAILURE);
  }
  printf("The program would test sending %d million messages\n", x_million);

  int shm_id; // id of the shared memory
  /* get the whole size of the shared memory. This first part of this the space to
  store sender's start time; the second part is for the ring buffer */
  size_t mem_size = sizeof(double) + sizeof(struct rbuffer);

  //Parent creates a shm first.
  void* mem_ptr =  create_global_common_area(mem_size, 1, &shm_id);

  int pid;
  int status;
  pid = fork();
  if ( pid < 0 ) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    //I'm child and I do receiving
    do_child_process(mem_size);
    exit(EXIT_SUCCESS);
  }
  else {
    //I'm parent and I do sending
    do_parent_process(mem_ptr, x_million);
    wait(&status); // wait for child to finish

    // detach the shared memory
    if (shmdt(mem_ptr) == -1) { //detach failed
      perror("Parent: Can't detach memory segment\n");
      exit(EXIT_FAILURE);
    }
    printf("Parent: detached memory segment\n");

    /* Remove the shared memory segment */
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) { // removing memory segment failed
      perror("Parent: Can't remove memory segment\n");
      exit(EXIT_FAILURE);
    }
    printf("Parent: removed memory segment\n");
    exit(EXIT_SUCCESS);
  }

  return 0; //make sure exit
}

/* create shared memory. Parent process has a different behavior from child since
  they use difference shmget() flags 
  Return: void*: the pointer to the shared memory 
  Params: size_t mem_size: tells how much size we need for the shared memory
          int is_parent: distinguishes if the caller process is parent or not
          int* shm_id: pointer to shm_id, used to do shm controls later 
*/
void* create_global_common_area(size_t mem_size, int is_parent, int* shm_id) {
  void *shm;
  key_t key = 6666;
  int shm_flg = 0;

  if (is_parent) {
    shm_flg = IPC_CREAT | 0666;
  }

  //get memory segment; parent and child use different shm_flg.
  if ((*shm_id = shmget(key, sizeof(mem_size), shm_flg)) < 0) {
    printf("Error: shmget() failed\n");
    exit(EXIT_FAILURE);
  }
  // attach the shared memory
  if ((shm = shmat(*shm_id, NULL, 0)) == (void *) - 1) {
    printf("Error: shmat() failed\n");
    exit(EXIT_FAILURE);
  }

  return shm; //void * type
}


/* this is a function parent process would execute, including: set up struct's pointers, 
  set up random number generator, while loop to send the messages and send the end 
  message, sum all sent numbers
  Return: void
  Params: void* : the pointer to the shared memory we get earlier
          unsigned int: how manay millions messages we want to send
*/
void do_parent_process(void* mem_ptr, unsigned int x_million) {
  if ( !mem_ptr )
  {
    printf("do_parent_process: invalid pointer\n");
    return;
  }

  /* cast to double, because the first part of this the space to 
  store sender's start time; the second part is for the ring buffer */
  double* time_buff = (double*) mem_ptr;
  struct rbuffer* g_area = (struct rbuffer*) ((size_t)mem_ptr + sizeof(double));
  rb_init(g_area);

  /* initialize random number generator */
  time_t t;
  int r;
  srand((unsigned) time(&t));
  // set up counter
  unsigned long sum = 0;
  unsigned long counter = x_million * 1000000;

  // get and write the start time
  *time_buff = get_cur_time();
  // printf("Sender: starts at %f\n", *(time_buff));
  while (counter--) {
    r = rand(); // between 0 and RAND_MAX
    int status = rb_add(g_area, r);
    if (status < 0)
    {
      printf("Write Error\n");
      break;
    }
    // printf("Write: %d\n", r);
    sum += r;
  }
  // write the end
  int status = rb_add(g_area, -1);
  if (status < 0) {
    printf("Write End Error\n");
    exit(EXIT_FAILURE);
  }
  printf("Sent sum: %lu\n", sum);

}


/* this is a function child process would execute, including: get the shared
  memory, set up struct's pointers, loop to receive the messages and sum all
  received numbers
  Return: void
  Params: size_t: the size of the shared memory we need
*/
void do_child_process(size_t mem_size) {
  int shm_id;
  void* mem_ptr = create_global_common_area(mem_size, 0, &shm_id);// get the shm pointer
  /* cast to double, because the first part of this the space to 
  store sender's start time; the second part is for the ring buffer */
  double* time_buff = (double*) mem_ptr;
  struct rbuffer* g_area = (struct rbuffer*) ((size_t)mem_ptr + sizeof(double));

  // initializers
  int read_status = -1;
  int read_data = 0;
  unsigned long sum = 0;
  unsigned long count = 0;

  // printf("Child reads from shm segment: %s", g_area->buffer);
  while (1) {
    // get one message from the ring buffer and save to variable read_data
    read_status = rb_remove(g_area, &read_data);
    if ( read_status < 0 )
    {
      //unable to get a message
      printf("Read Error\n");
      break;
    }
    if ( read_data < 0 )
    {
      // got the message indicating that sending is finished
      printf("Received the end(%d)\n", read_data);
      break;
    }
    // printf("Read: %d\n", read_data);
    sum += read_data;
    count++;
  }
  double time_diff = get_cur_time() - *time_buff; // get the duration from start of sending to end of receiving

  // print out informations
  printf("Get sum: %lu\n", sum);
  printf(">>>Sending starts at: %f s\n", *time_buff);
  printf(">>>Received: %lu messages\n", count);
  printf(">>>The whole duration: %f s\n", time_diff);
  printf(">>>Throughput: %.5f msg/s\n", count / time_diff);

  // detach shared memory
  if (shmdt(mem_ptr) == -1) {
    // detaching failed
    printf("Child: Can't detach memory segment\n");
    exit(1);
  }
  printf("Child: detached memory segment\n");

}

