// rbuffer.c
// Author: Yimou Cao
// Date: 11/6/2017
// Course: CSCI503
// Lab: #4

#include <stdlib.h>
#include "utils.h"
#include "sem.h"
#include "rbuffer.h"


/* Allocate memory for a ring buffer and initialize it. This process includes set values
of buffer size/length, head, tail, semaphores
Params: length: the size of the ring buffer
        num_all: the number of products to produce
Return: pointer to the ring buffer struct
*/
RBuffer* make_rbuffer(int length, int num_all)
{
  RBuffer* rbuffer = (RBuffer* ) malloc_check(sizeof(RBuffer));
  rbuffer->length = length;
  rbuffer->array = (int *) malloc_check(length * sizeof(int));
  rbuffer->head = 0;
  rbuffer->tail = 0;
  rbuffer->num_rest_to_produce = num_all;
  rbuffer->num_rest_to_consume = num_all;
  rbuffer->mutex = make_semaphore(1); // guarantee exclusive access
  rbuffer->items = make_semaphore(0); // items in queue (produced but not consumed yet)
  rbuffer->spaces = make_semaphore(length);
  return rbuffer;
}

/* Test if the ring buffer is empty
Params: rbuffer: the pointer to the ring buffer
Return: an integer: 1 is empty, 0 is not empty
*/
int rbuffer_empty(RBuffer* rbuffer)
{
  // rbuffer is empty if head - tail == 0
  int res = ((rbuffer->head - rbuffer->tail) == 0);
  return res;
}

/* Test if the ring buffer is full
Params: rbuffer: the pointer to the ring buffer
Return: an integer: 1 is full, 0 is not full
*/
int rbuffer_full(RBuffer* rbuffer)
{
  // rbuffer is full if incrementing next_in lands on next_out
  int res = ((rbuffer->head - rbuffer->tail) == rbuffer->length);
  return res;
}

/* Push an item into the ring buffer. Get relevant semophores firstly, 
then get to know if there's task to do. If not, release semophores and
write objective-reached status. Otherwise, write the item into array of
the ring buffer and release semophores
Params: rbuffer: the pointer to the ring buffer
        item: an item just produced by a producer
        status: pointer to an integer which indicate if objective is reached
Return: no return
*/
void rbuffer_push(RBuffer* rbuffer, int item, int* status) {
  
  semaphore_P(rbuffer->spaces);
  semaphore_P(rbuffer->mutex);

  if (--(rbuffer->num_rest_to_produce) < 0) // if no task to do
  {
    ++(rbuffer->num_rest_to_produce);  //change it back to 0
    *status = 1;  // objective reached, no need to produce anymore
    semaphore_V(rbuffer->mutex);
    semaphore_V(rbuffer->spaces);
  } 
  else
  {
    unsigned index = (rbuffer -> head) % rbuffer->length;
    rbuffer->array[index] = item; // write value
    (rbuffer -> head)++;
    *status = 0;

    semaphore_V(rbuffer->mutex);
    semaphore_V(rbuffer->items);
  }
  
  return;
}

/* Pop an item into the ring buffer. Get relevant semophores firstly, 
then get to know if producers stopped producing and buffer is empty at
the same time. If yes, release semophores and write objective-reached 
status. Otherwise, write the item the value at the tail of the array of
the ring buffer and release semophores. If number of products rest to 
consume is <= 0, it is necessary to wake other consumers that're in the
waiting state because of no items available.
Params: rbuffer: the pointer to the ring buffer
        item: an item just produced by a producer
        status: pointer to an integer which indicate if objective is reached
          (job done and no item in buffer)
Return: no return
*/
void rbuffer_pop(RBuffer* rbuffer, int* item, int* status) {

  semaphore_P(rbuffer->items);
  semaphore_P(rbuffer->mutex);
  
  // if (rbuffer->num_rest_to_produce <=0 && rbuffer->items->value <= 0){
  if (rbuffer->num_rest_to_produce <=0 && rbuffer_empty(rbuffer)){
    *status = 1;
    semaphore_V(rbuffer->mutex);
    semaphore_V(rbuffer->items);
    return;
  }

  unsigned index = (rbuffer -> tail) % rbuffer->length;
  *item = rbuffer->array[index];
  (rbuffer -> tail)++;

  if ( --(rbuffer->num_rest_to_consume) <= 0)
  {
    semaphore_V(rbuffer->items); // wake other consumers so as to let others jump out(return)
  }


  semaphore_V(rbuffer->mutex);
  semaphore_V(rbuffer->spaces);

  *status = 0;
  return;
}