// utils.c
// Author: Yimou Cao
// Date: 11/6/2017
// Course: CSCI503
// Lab: #4
// utility functions for the program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"

/* perror out the error message s and exit with code -1
Params: char* s: the pointer to the error message
Return: void: no return
*/
void perror_exit(char* s)
{
  perror(s);  
  exit(-1);
}

/* malloc a chunk of memory with size and check if this operation succeeds or not
 If succeeds, return the pointer. Otherwise, exit with code -1
 Params: int size, the size of memory needed
 Return: void*: the pointer to the allocated memory
*/
void* malloc_check(int size)
{
  void* p = malloc(size);
  if (p == NULL) perror_exit("malloc failed");
  return p;
}


/* utility functions for mutex */
/*
 malloc and init mutex, and check validility of the mutex
 if valid, return the mutex. otherwise exit
 Params: no input params
 Return: Mutex, type of pthread_mutex_t
*/
Mutex* make_mutex()
{
  Mutex* mutex = malloc_check(sizeof(Mutex));
  int n = pthread_mutex_init(mutex, NULL);
  if (n != 0) perror_exit("make_mutex() failed"); 
  return mutex;
}

/*
Invoke pthread_mutex_lock function and check if this operation success or not
if success return else exit
Params: a Mutex from make_mutex()
Return: no return
*/
void mutex_lock(Mutex* mutex)
{
  int n = pthread_mutex_lock(mutex);
  if (n != 0) perror_exit("mutex_lock() failed");
}

/*
Invoke pthread_mutex_unlock function and check if this operation success or not
if success return else exit
Params: a Mutex from make_mutex()
Return: no return
*/
void mutex_unlock(Mutex* mutex)
{
  int n = pthread_mutex_unlock(mutex);
  if (n != 0) perror_exit("mutex_unlock() failed");
}

/* END utility functions for mutex */

/* utility functions for conditional varialbe */

/*
 malloc and init a condition, and check validility of the a condition
 if valid, return the a condition. otherwise exit
 Params: no input params
 Return: Mutex, type of pthread_cond_t
*/
CV* make_cv()
{
  CV* cv = malloc_check(sizeof(CV)); 
  int n = pthread_cond_init(cv, NULL);
  if (n != 0) perror_exit("make_cv() failed");
 
  return cv;
}

/*
Invoke pthread_cond_wait function and check if this operation success or not
if success return else exit
Params: cv, conditional variable on which the wait is operated
		mutex, a corresponding mutex for the conditional variable
Return: no return
*/
void cv_wait(CV* cv, Mutex* mutex)
{
  int n = pthread_cond_wait(cv, mutex);
  if (n != 0) perror_exit("cv_wait() failed");
}

/*
Invoke pthread_cond_wait function and check if this operation success or not
if success return else exit
Params: cv, conditional variable on which the signal is operated
Return: no return
*/
void cv_signal(CV *cv)
{
  int n = pthread_cond_signal(cv);
  if (n != 0) perror_exit("cv_signal() failed");
}

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
int parse_args(int argc, char* argv[], int* num_producers,int* num_consumers,int* max_sleep_seconds,int* num_products,int* rbuffer_size)
{
	if (argc == 1)
	{
		return 0; // use default args
	}
	else if (argc == 6)
	{
		int a,b,c,d,e;
		a = atoi(argv[1]);
		b = atoi(argv[2]);
		c = atoi(argv[3]);
		d = atoi(argv[4]);
		e = atoi(argv[5]);

		if (a <= 0 || b <= 0 || c <= 0|| d <= 0|| e <= 0)
		{
			return -1;
		}

		*num_producers = atoi(argv[1]);
		*num_consumers = atoi(argv[2]);
		*max_sleep_seconds = atoi(argv[3]);
		*num_products = atoi(argv[4]);
		*rbuffer_size = atoi(argv[5]);

		return 1;
	}
	else
	{
		return -1; // Incorrect number of args
	}
}