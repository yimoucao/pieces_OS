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
#include <sys/mman.h>
#include <fcntl.h> //O_CREAT etc. tags

#include "utils.h"
#include "ringbuffer.h"

#define SHM_NAME "POSIX_SHM" /* the shared memory name */
#define X_MILLION 1 /* how many millions of message this program would sent by default*/

/* declarations of functions*/
/* this is a function parent process would execute, including: mapping the shared memory to
  current memory space, set up struct's pointers, set up random number generator, while loop
  to send the messages and send the end message, sum all sent numbers
  Return: void
  Params: int: the file descriptor of the shared memory
          size_t: the size of the shared memory we need
          unsigned int: how many millions of messages to be sent
*/
void do_parent_process(int, size_t, unsigned int);

/* this is a function child process would execute, including: mapping the shared memory to
  current memory space, set up struct's pointers, loop to receive the messages and sum all
  received numbers
  Return: void
  Params: size_t: the size of the shared memory we need
*/
void do_child_process(size_t);



/* main entrance of the application */
int main(int argc, char* argv[]) 
{
	/* parse args from the command line */
	unsigned int x_million = X_MILLION; // set it as the default value
	int parse_status = parse_args(argc, argv, &x_million);
    if ( parse_status < 0)
    {
        exit(EXIT_FAILURE);
    }
    printf("The program would test sending %d million messages\n", x_million);

	int pid;
	int status; // for waitting child
	int shm_fd; // file descriptor of the shared memory

	//Parent creates a shm first.
	if ((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) < 0) {
		printf("Parent: Error: shm_open() failed\n");
		exit(EXIT_FAILURE);
	}

	/* get the whole size of the shared memory. This first part of this the space to 
	store sender's start time; the second part is for the ring buffer */
	size_t mem_size = sizeof(double) + sizeof(struct rbuffer);
	ftruncate(shm_fd, mem_size); /* set up the shared memory size */

	pid = fork();

	if( pid < 0 ) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0){ 
		//I'm child and I do receiving
		do_child_process(mem_size);
		exit(EXIT_SUCCESS);
	}
	else { 
		//I'm parent and I do sending
		do_parent_process(shm_fd, mem_size, x_million);
		wait(&status);
		shm_unlink(SHM_NAME); ///* delete shared memory file */
		exit(EXIT_SUCCESS);
	}

	return 0; //make sure exit
}



/* this is a function parent process would execute, including: mapping the shared memory to
  current memory space, set up struct's pointers, set up random number generator, while loop
  to send the messages and send the end message, sum all sent numbers
  Return: void
  Params: int: the file descriptor of the shared memory
          size_t: the size of the shared memory we need
          unsigned int: how many millions of messages to be sent
*/
void do_parent_process(int shm_fd, size_t mem_size, unsigned int x_million) {

	/* map to current address space */
	void* ptr = mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if ( ptr == (void *)-1)
	{
		printf("Parent: Error: mmap() failed\n");
		return;
	}
	double* time_buff = (double*) ptr;
	struct rbuffer* g_area = (struct rbuffer*) (ptr + sizeof(double));
    rb_init(g_area); // intialize the ring buffer

	/* initialize random number generator */
	time_t t;
	int r;
	srand((unsigned) time(&t));
	// set up counter
    unsigned long sum = 0;
    unsigned long counter = x_million * 1000000;
    // counter = 5;

    // get and write the start time
    *time_buff = get_cur_time();
    // printf("Sender: starts at %f\n", *(time_buff));
	while(counter--){
		r = rand(); // between 0 and RAND_MAX
		int status = rb_add(g_area, r);
		if (status < 0){
			printf("Write Error\n");
			break;
		}
		// printf("Write: %d\n", r);
		sum += r;
		
	}
	// write the end of transmission
	int status = rb_add(g_area, -1);
	if (status < 0){
		printf("Write End Error\n");
		exit(EXIT_FAILURE);
	}
	printf("Sent sum: %lu\n", sum);

}


/* this is a function child process would execute, including: mapping the shared memory to
  current memory space, set up struct's pointers, loop to receive the messages and sum all
  received numbers
  Return: void
  Params: size_t: the size of the shared memory we need
*/
void do_child_process(size_t mem_size) {
	
	int shm_fd; /* the shared memory descriptor */
	/* open */
	if ((shm_fd = shm_open(SHM_NAME, O_RDWR, 0666)) < 0)
	{
		printf("Child: Error: shm_open() failed\n");
		exit(EXIT_FAILURE);
	}
	/* mapping to the current address space */
	void* ptr = mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if ( ptr == (void *)-1) // mappin failed
	{
		printf("Child: Error: mmap() failed\n");
		return;
	}
	// set up pointers
	double* time_buff = (double*) ptr;
	struct rbuffer* g_area = (struct rbuffer*) (ptr + sizeof(double));

	// initialize some variables
	int read_status = -1;
	int read_data = 0;
	unsigned long sum = 0;
	unsigned long count = 0;


	while(1){
		// get one message from ring buffer and save to variable read_data
		read_status = rb_remove(g_area, &read_data);
		if ( read_status < 0 )
		{	// unable to get a message
			printf("Read Error\n");
			break;
		}
		if ( read_data < 0 )
		{
			// got the message indicating sending is finished
			printf("Received the end(%d)\n", read_data);
			break;
		}
		// printf("Read: %d\n", read_data);
		sum += read_data; // sum all the integers
		count++; // count how many messages are received
	}
	double time_diff = get_cur_time() - *time_buff; // get the duration from start of sending to end of receiving

	// print out informations
	printf("Get sum: %lu\n", sum);
    printf(">>>Sending starts at: %f s\n", *time_buff);
    printf(">>>Received: %lu messages\n", count);
    printf(">>>The whole duration: %f s\n", time_diff);
    printf(">>>Throughput: %.5f msg/s\n", count/time_diff);

}
