/*
// parmul.h
// functions for parallel multiplication of two matrix
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/


#ifndef _PARMUL_H
#define _PARMUL_H

#include <utils.h>

/* Define a new type called TaskBoard, which controls the whole process of distributing
tasks to each process. For a NxN matrix, there're N*N tasks for each elements. The mutex
of the taskboard makes sure that each task is carried out only once.
*/
typedef struct {
	unsigned long dimension, size;
	unsigned* board;
	Mutex* mutex;
} TaskBoard;

/* Make a TaskBoard for matrices with dimension. This includes initializing the dimension,
the size of the taskboard, the pointer to an array with values 1 that meaning all tasks are
available, and the mutex
Params: the rank of those square matrices
Return: the pointer to a TaskBoard struct
*/
TaskBoard* make_taskboard(unsigned long dimension);



/* 
Define a new type called Shared, which is passed to entry function of each thread.This struct
includes dimension of all its matrices, two matrices to be multiplied and one result matrix, 
and a corresponding task_board.
*/
typedef struct 
{
	unsigned long dimension;
	double** matrixA;
	double** matrixB;
	double** matrixC;
	TaskBoard* task_board;
} Shared;

/* Make a Shared struct for all threads. This includes initializing the dimension,
pointers to matrices and task_board
Params: int dimension, the rank of those square matrices
		double** matrixA, pointer to matrixA
		double** matrixB, pointer to matrixB
		double** matrixC, pointer to the result matrixC
		TaskBoard* task_board, pointer to the TaskBoard for these matrices
Return: the pointer to a Shared struct
*/
Shared* make_shared(int dimension, double** matrixA, double** matrixB, double** matrixC, TaskBoard* task_board);


/* Get a task from the task_board, row and col are the pointer to the objective vectors respectively
status tell us if all tasks are taken, 1 is yes, 0 is no
Params: TaskBoard* task_board, the pointer to a TaskBoard struct
		int* row, pointer to the number of the objective row
		int* col, pointer to the number of the objective column
		int* status, the status of non-availability of tasks, 1 no availables, 0 available
Return: int, 1, got a task and going to carry out the task; 0, no taks to do
*/
int getTask(TaskBoard* task_board, int* row, int* col, int* status);

/* the entry function for each thread */
void* worker_entry(void* arg);

/* THe main procedure of the parallel multiplication 
Params: double** matrixA, pointer to matrixA
		double** matrixB, pointer to matrixB
		double** matrixC, pointer to the result matrixC
		int dimension, the rank of those square matrices
		int n_threads, number of threads needed to accomplish the tasks
Return: 1, success; if error happended the procudure would exit
*/
int parallelMultiply(double** matrixA, double** matrixB, double** matrixC, int dimension, int n_threads);

#endif /* _PARMUL_H */