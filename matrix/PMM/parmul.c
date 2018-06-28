/*
// parmul.h
// Implementations of functions for parallel multiplication of two matrix
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "parmul.h"
#include "utils.h"


/* Operation functions for TaskBoard */
/* Make a TaskBoard for matrices with dimension. This includes initializing the dimension,
the size of the taskboard, the pointer to an array with values 1 that meaning all tasks are
available, and the mutex
Params: dimension, the rank of those square matrices
Return: the pointer to a TaskBoard struct
*/
TaskBoard* make_taskboard(unsigned long dimension){
	TaskBoard* task_board = malloc_check(sizeof(TaskBoard));
	task_board->dimension = dimension;
	task_board->size = dimension * dimension;
	task_board->board = malloc_check(task_board->size * sizeof(unsigned));
	task_board->mutex = make_mutex();

	int i;
	for (i = 0; i < task_board->size; ++i)
	{
		task_board->board[i] = 1;
	}
	return task_board;
}
/* END Operation functions for TaskBoard */


/* Operation functions for Shared */
/* Make a Shared struct for all threads. This includes initializing the dimension,
pointers to matrices and task_board
Params: int dimension, the rank of those square matrices
		double** matrixA, pointer to matrixA
		double** matrixB, pointer to matrixB
		double** matrixC, pointer to the result matrixC
		TaskBoard* task_board, pointer to the TaskBoard for these matrices
Return: the pointer to a Shared struct
*/
Shared* make_shared(int dimension, double** matrixA, double** matrixB, double** matrixC, TaskBoard* task_board){
	Shared* shared = malloc_check(sizeof(Shared));
	shared->dimension = dimension;
	shared->matrixA = matrixA;
	shared->matrixB = matrixB;
	shared->matrixC = matrixC;
	shared->task_board = task_board;
	return shared;
}
/* END Operation functions for Shared */


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

/* Get a task from the task_board, row and col are the pointer to the objective vectors respectively
status tell us if all tasks are taken, 1 is yes, 0 is no
Params: TaskBoard* task_board, the pointer to a TaskBoard struct
		int* row, pointer to the number of the objective row
		int* col, pointer to the number of the objective column
		int* status, the status of non-availability of tasks, 1 no availables, 0 available
Return: int, 1, got a task and going to carry out the task; 0, no taks to do
*/
int getTask(TaskBoard* task_board, int* row, int* col, int* status){

	int i=0;
	int all_tested = 0;
	int n = task_board->size;
	int dimension = task_board->dimension;
	unsigned long num = (unsigned long)rand();
	unsigned long index = num  % n;
	
	//mutex_start
	mutex_lock(task_board->mutex);
	while(task_board->board[index]==0){ // 0 not available, 1 availabe
		index = (++num) % n;
		if ( (++i) >= n)
		{
			all_tested = 1;
			*status = all_tested;
			mutex_unlock(task_board->mutex);
			return 0; // no task to do anymore
		}
	}
	task_board->board[index] = 0; 
	mutex_unlock(task_board->mutex);
	//mutex_end

	*row = index / dimension;
	*col = index % dimension;
	return 1; // having a task to do
}

/* the entry function for each thread */
void* worker_entry(void* arg)
{
	//parse out matrixes
	Shared* shared = (Shared* ) arg;
	int dimension = shared->dimension;
	double** matrixA = shared->matrixA;
	double** matrixB = shared->matrixB;
	double** matrixC = shared->matrixC;
	TaskBoard* task_board =  shared->task_board;

	srandom(time(0)+clock()+random());
	//prepare
	int row = -1, col = -1;
	int status;
	while(getTask(task_board, &row, &col, &status)){
		if ( row < 0 || col < 0)
		{
			fprintf(stderr, "Thread[%lu] get an error task [%d, %d]\n", (long)pthread_self(), row, col);
		}
		printf("Thread[%lu] got a task. Compute [%d, %d]\n", (long)pthread_self(), row, col);
		int k;
		int total=0;
		for (k = 0; k < dimension; ++k)
		{
			total += matrixA[row][k] * matrixB[k][col];
		}
		matrixC[row][col] = total;
	}
	printf("Thread[%lu] no task to do\n", (long)pthread_self());
	pthread_exit(NULL);
}

/* THe main procedure of the parallel multiplication 
Params: double** matrixA, pointer to matrixA
		double** matrixB, pointer to matrixB
		double** matrixC, pointer to the result matrixC
		int dimension, the rank of those square matrices
		int n_threads, number of threads needed to accomplish the tasks
Return: 1, success; if error happended the procudure would exit
*/
int parallelMultiply(double** matrixA, double** matrixB, double** matrixC, int dimension, int n_threads){
	TaskBoard* task_board = make_taskboard(dimension);
	Shared* shared = make_shared(dimension, matrixA, matrixB, matrixC, task_board);

	pthread_t workers[n_threads];
	int i;
	for (i = 0; i < n_threads; ++i)
	{
		workers[i] = make_thread(worker_entry, shared);
	}


	for (i = 0; i < n_threads; ++i)
	{
		join_thread(workers[i]);
	}

	return 1;
}