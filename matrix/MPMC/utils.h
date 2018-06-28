// utils.h
// Author: Yimou Cao
// Date: 11/6/2017
// Course: CSCI503
// Lab: #4
/* utility functions for the program, including wrapper function for
perror(), malloc(), mutex and conditional variable, and the parse
function to parse command line arguments
*/

#ifndef UTILS_H
#define UTILS_H

/* perror out the error message s and exit with code -1
Params: char* s: the pointer to the error message
Return: void: no return
*/
void perror_exit(char* s);

/* malloc a chunk of memory with size and check if this operation succeeds or not
 If succeeds, return the pointer. Otherwise, exit with code -1
 Params: int size, the size of memory needed
 Return: void*: the pointer to the allocated memory
*/
void* malloc_check(int size);  // malloc and check


/* utility functions for mutex */

typedef pthread_mutex_t Mutex;

/*
 malloc and init mutex, and check validility of the mutex
 if valid, return the mutex. otherwise exit
 Params: no input params
 Return: Mutex, type of pthread_mutex_t
*/
Mutex* make_mutex();

/*
Invoke pthread_mutex_lock function and check if this operation success or not
if success return else exit
Params: a Mutex from make_mutex()
Return: no return
*/
void mutex_lock(Mutex* mutex);

/*
Invoke pthread_mutex_unlock function and check if this operation success or not
if success return else exit
Params: a Mutex from make_mutex()
Return: no return
*/
void mutex_unlock(Mutex* mutex);
/* END utility functions for mutex */


/* utility functions for conditional varialbe */

typedef pthread_cond_t CV;

/*
 malloc and init a condition, and check validility of the a condition
 if valid, return the a condition. otherwise exit
 Params: no input params
 Return: Mutex, type of pthread_cond_t
*/
CV* make_cv();

/*
Invoke pthread_cond_wait function and check if this operation success or not
if success return else exit
Params: cv, conditional variable on which the wait is operated
		mutex, a corresponding mutex for the conditional variable
Return: no return
*/
void cv_wait(CV* cv, Mutex* mutex);

/*
Invoke pthread_cond_wait function and check if this operation success or not
if success return else exit
Params: cv, conditional variable on which the signal is operated
Return: no return
*/
void cv_signal(CV* cv);
/* END utility functions for conditional varialbe */


/*
Get parameters from command line and parse them to integers
The behavior is same as atoi()
Params: argc, from main function
		argv, from main function
		num_producers, pointer to a integer of number of producers
		num_consumers, pointer to a integer of number of consumers
		max_sleep_seconds, pointer to a integer of number of max_sleep_seconds
		num_products, pointer to a integer of number of products of all to produce
		rbuffer_size, pointer to a integer of number of the ring buffer size
Return: 0: no change to integers above
		-1: error happened, args are invalid or too many args
		1: values from the command are written to those integers
*/
int parse_args(int argc, char* argv[], int* num_producers,int* num_consumers,int* max_sleep_seconds,int* num_products,int* rbuffer_size);

#endif /* UTILS_H */