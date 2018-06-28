/*
FILE: utils.h
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/

#ifndef _UTILS_H
#define _UTILS_H

#include <pthread.h>

void perror_exit(char* s);
void* malloc_check(int size);


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




int parse_args(int argc, char* argv[], int* n_elevators,int* n_floors,
  int* people_speed,int* elevator_speed,int* simulation_time,unsigned* seed);

#endif /* _UTILS_H */