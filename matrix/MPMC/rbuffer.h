// rbuffer.h
// Author: Yimou Cao
// Date: 11/6/2017
// Course: CSCI503
// Lab: #4


#ifndef RBUFFER_H
#define RBUFFER_H

typedef struct {
  int *array;
  int length;
  unsigned head;
  unsigned tail;
  int num_rest_to_produce;
  int num_rest_to_consume;
  Semaphore* mutex;
  Semaphore* items;
  Semaphore* spaces;
} RBuffer;


/* Allocate memory for a ring buffer and initialize it. This process includes set values
of buffer size/length, head, tail, semaphores
Params: length: the size of the ring buffer
        num_all: the number of products to produce
Return: pointer to the ring buffer struct
*/
RBuffer* make_rbuffer(int length, int num_all);


/* Test if the ring buffer is empty
Params: rbuffer: the pointer to the ring buffer
Return: an integer: 1 is empty, 0 is not empty
*/
int rbuffer_empty(RBuffer* rbuffer);

/* Test if the ring buffer is full
Params: rbuffer: the pointer to the ring buffer
Return: an integer: 1 is full, 0 is not full
*/
int rbuffer_full(RBuffer* rbuffer);

/* Push an item into the ring buffer. Get relevant semophores firstly, 
then get to know if there's task to do. If not, release semophores and
write objective-reached status. Otherwise, write the item into array of
the ring buffer and release semophores
Params: rbuffer: the pointer to the ring buffer
        item: an item just produced by a producer
        status: pointer to an integer which indicate if objective is reached
Return: no return
*/
void rbuffer_push(RBuffer* rbuffer, int item, int* status);

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
void rbuffer_pop(RBuffer* rbuffer, int* item, int* status); // status=1, job done and no item in buffer

#endif /* RBUFFER_H */