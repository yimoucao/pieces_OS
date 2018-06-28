/** 
 * @file ringbuffer.h
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

// define ring buffer status codes
#define R_BUFF_FAILURE (-1)
#define R_BUFF_FULL (-31)
#define R_BUFF_EMPTY (-32)

// define the ring buffer size
#define R_BUFFER_SIZE 4096

// define the struct of the ring buffer
struct rbuffer
{
	unsigned head;
	unsigned tail;
	int data[R_BUFFER_SIZE];
};

/* add one item to the ring buffer. If the buffer is full it starts waiting
  Return: int: status code. 0 means success, negative means error
  Params: ptr: pointer to the ring buffer
          int new: the value needs to be added to the ring buffer
 */
int rb_add(struct rbuffer* ptr, int a);

/* remove one item to the ring buffer. If the buffer is empty it starts waiting
  Return: int: status code. 0 means success, negative means error
  Params: ptr: pointer to the ring buffer
          int* item: pointer to a variable that need the removed value
 */
int rb_remove(struct rbuffer* ptr, int* item);

/* initialize the ring buffer. set the ring buffer is empty, which means
  that head is equal to tail
  Return: int: status code. success 0; negative failure
  Params: ptr: pointer to the ring buffer*/
int rb_init(struct rbuffer* ptr);

#endif /* RINGBUFFER_H */