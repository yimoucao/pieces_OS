// main.c
// Author: Yimou Cao
// Date: 11/6/2017
// Course: CSCI503
// Lab: #4

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include "sem.h"
#include "rbuffer.h"

#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 1
#define MAX_SLEEP_SECONDS 1
#define NUM_ITEMS 32
#define RBUFFER_SIZE 128

#define SLEEP_TIME_OFFSET (1) // change sleep time range to [0+offset, max-1+offset]

// define the struct of Shared, used to pass into different threads
typedef struct {
  int produced; // count how many products produced by producers
  int consumed; // count how many products consumed by consumers
  Mutex* produced_mutex; // mutex for the variable produced
  Mutex* consumed_mutex; // mutex for the variable consumed
  RBuffer* rbuffer; // ring buffer used to store products
} Shared;


/* Make a Shared struct for all threads. This includes initializing the ring buffer,
counters and mutexes.
Params: int rbuffer_size, the size of the ring buffer
    int num_all, the number of products to produce
Return: the pointer to a Shared struct
*/
Shared* make_shared(int rbuffer_size, int num_all)
{
	// TODO: check size range
  Shared* shared = malloc_check(sizeof(Shared));
  shared->rbuffer = make_rbuffer(rbuffer_size, num_all);
  shared->produced = 0;
  shared->consumed = 0;
  shared->produced_mutex = make_mutex();
  shared->consumed_mutex = make_mutex();
  return shared;
}

/* Update the variable that counts how many products made by a thread
Since the varialbe is a shared resource between all threads, a lock is
required before updating the varialbe and releasing after it.
Params: Shared* shared, the pointer to the shared struct
    int value, the number of products the thread produced
Return: no return
*/
void update_produced(Shared* shared, int value){
  if ( shared == NULL || shared->produced_mutex == NULL)
  {
    perror_exit("invalid pointer to Shared struct / mutex for variable produced\n");
  }
  if ( value < 0 )
  {
    perror_exit("invalid value of number of produced of a certain thread\n");
  }
  mutex_lock(shared->produced_mutex);
  shared->produced += value;
  mutex_unlock(shared->produced_mutex);
}

/* Update the variable that counts how many products consumded by a thread
Since the varialbe is a shared resource between all threads, a lock is
required before updating the varialbe and releasing after it.
Params: Shared* shared, the pointer to the shared struct
    int value, the number of products the thread consumed
Return: no return
*/
void update_consumed(Shared* shared, int value){
  if ( shared == NULL || shared->consumed_mutex == NULL)
  {
    perror_exit("invalid pointer to Shared struct / mutex for variable consumed\n");
  }
  if ( value < 0 )
  {
    perror_exit("invalid value of number of consumed of a certain thread\n");
  }
  mutex_lock(shared->consumed_mutex);
  shared->consumed += value;
  mutex_unlock(shared->consumed_mutex);
}

/* Wrapper functions on thread */

pthread_t make_thread(void *(*entry)(void *), Shared* shared)
{
  int ret;
  pthread_t thread;

  ret = pthread_create(&thread, NULL, entry, (void *) shared);
  if (ret != 0) {
      perror_exit("pthread_create failed");
  }
  return thread;
}

void join_thread(pthread_t thread)
{
  int ret = pthread_join(thread, NULL);
  if (ret != 0) {
      perror_exit("pthread_join failed");
  }
}
/* END Wrapper functions on thread */


/* Hanle sleep random secodns */
/* Operator on max_sleep_seconds. If set == 1, this function would set max_sleep_seconds
  to the value passed in. If set != 1, this function only returns the max_sleep_seconds
Params: int set: variable indicating if setting new value is desired
Return: max_sleep_seconds
*/
int op_max_sleep_seconds(int set, int value)
{
	static int max_sleep_seconds = MAX_SLEEP_SECONDS;
	if (set) // set == 1, set new max_sleep_seconds
	{
		max_sleep_seconds = value;
	}
	// if set != 1, only read value
	return max_sleep_seconds;
}

/* Sleep a random number of seconds in [1, max_sleep_seconds] 
Params: no params
Return: no return
*/
void thread_sleep_rands()
{
  int r = (rand() % op_max_sleep_seconds(0, 0)) + SLEEP_TIME_OFFSET;
 	sleep(r);
}
/* END hanle sleep random secodns */


// PRODUCER-CONSUMER
void* producer_entry(void* arg)
{
  int i=0;
  int status_full = 0;
  Shared* shared = (Shared* ) arg;
  while(1) {
  	thread_sleep_rands(); // sleep random seconds
    rbuffer_push(shared->rbuffer, i, &status_full);
    if (status_full)
    {// Although I produce a product, but the objective is reached so I didn't push it into buffer just now.
    	break;
    } 
    printf("added item %d\n", i);
    i++;
  }
  printf("Producer[%lu] finished. It produced %d items.\n", (long)pthread_self(), i);
  update_produced(shared, i);
  pthread_exit(NULL);
}

void* consumer_entry(void* arg)
{
  int i=0;
  int item, status_jobdone;
  Shared* shared = (Shared* ) arg;

  while(1) {
  	thread_sleep_rands(); // sleep random seconds
    rbuffer_pop(shared->rbuffer, &item, &status_jobdone);
    if (status_jobdone)
    {
    // producers dont't produce anymore and no pruducts in buffer
    	break; 
	 }
    printf("consumed item %d\n", item);
    i++;
  }
  printf("Consumer[%lu] finished. It consumed %d items.\n", (long)pthread_self(), i);
  update_consumed(shared, i);
  pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
	// rbuffer_test(); return 0;

	int num_producers = NUM_PRODUCERS;
	int num_consumers = NUM_CONSUMERS;
	int max_sleep_seconds = MAX_SLEEP_SECONDS;
	int num_products = NUM_ITEMS;
	int rbuffer_size = RBUFFER_SIZE;
	int status = parse_args(argc, argv, &num_producers,&num_consumers,&max_sleep_seconds,&num_products,&rbuffer_size);
	if ( status < 0)
	{
		printf("Incorrect number of args; Or Invalid arg value\n");
		exit(-1);
	}

	printf("No. Producers: %5d\nNo. Consumers: %5d\nMax Sleep Seconds: %d\nNo. Products All: %5d\nRingBuffer Size: %5d\n", 
		num_producers,num_consumers,max_sleep_seconds,num_products,rbuffer_size);

  int i, j;

  pthread_t producers[num_producers];
  pthread_t consumers[num_consumers];
  op_max_sleep_seconds(1, max_sleep_seconds);

  Shared* shared = make_shared(rbuffer_size, num_products);

  srand(time(NULL)); // seed the random generator; 

  for (i = 0; i < num_producers; ++i)
  {
  	producers[i] = make_thread(producer_entry, shared);
  }
  for (j = 0; j < num_consumers; ++j)
  {
  	consumers[j] = make_thread(consumer_entry, shared);
  }

  // main thread wait for all producer threads
  for (i = 0; i < num_producers; ++i)
  {
  	join_thread(producers[i]);
  }
  // main thread wait for all consumer threads
  for (j = 0; j < num_consumers; ++j)
  {
  	join_thread(consumers[j]);
  }

  //print our summanry
  printf("In summary:\n");
  printf("All producers produced: %d items\n", shared->produced);
  printf("All comsumers comsumed: %d items\n", shared->consumed);

  free(shared);
  return 0;
}