/** 
 * @file ringbuffer.c
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */

// ring buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "ringbuffer.h"


/* initialize the ring buffer. set the ring buffer is empty, which means
  that head is equal to tail
  Return: int: status code. success 0; negative failure
  Params: ptr: pointer to the ring buffer*/
int rb_init(struct rbuffer* ptr){
	if ( !ptr )
	{
		printf("Error: Buffer pointer is invalid\n");
		return R_BUFF_FAILURE; // failure
	}
	ptr->head = ptr->tail;
	return 0;
}

/* add one item to the ring buffer. If the buffer is full it starts waiting
  Return: int: status code. 0 means success, negative means error
  Params: ptr: pointer to the ring buffer
  		  int new: the value needs to be added to the ring buffer
 */
int rb_add(struct rbuffer* ptr, int new){
	if ( !ptr )
	{
		printf("Error: Buffer pointer is invalid\n");
		return R_BUFF_FAILURE; // failure
	}
	// if (ptr->head - ptr->tail == R_BUFFER_SIZE)
	// {
	// 	printf("Error: Buffer is full\n");
	// 	return R_BUFF_FULL; //buffer is full
	// }
	while(ptr->head - ptr->tail == R_BUFFER_SIZE) ; // full, waiting
	unsigned index = (ptr -> head) % R_BUFFER_SIZE; // get the buffer index for the new item
	ptr->data[index] = new; //store it to the buffer
	__sync_fetch_and_add(&(ptr->head), 1); // synchronously fetch and add the head

	return 0; //success
}

/* remove one item to the ring buffer. If the buffer is empty it starts waiting
  Return: int: status code. 0 means success, negative means error
  Params: ptr: pointer to the ring buffer
  		  int* item: pointer to a variable that need the removed value
 */
int rb_remove(struct rbuffer* ptr, int* item){
	if ( !ptr )
	{
		printf("Error: Buffer pointer is invalid\n");
		return R_BUFF_FAILURE;
	}
	// if (ptr->head - ptr->tail == 0)
	// {
	// 	printf("Error: Buffer is empty\n");
	// 	return R_BUFF_EMPTY; // buffer is empty
	// }
	while(ptr->head - ptr->tail == 0) ; // empty, waiting
	unsigned index = (ptr->tail) % R_BUFFER_SIZE; // get the buffer index for the new item
	*item = ptr->data[index]; //store it to a variable
	__sync_fetch_and_add(&(ptr->tail), 1); // synchronously fetch and add the tail
	return 0; //success
}