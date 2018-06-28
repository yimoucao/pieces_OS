/*
// utils.h
// utility functions for the program
including error printing out, malloc and check, wrapper functions for mutex
and matrix generating and comparison
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/

#ifndef _UTILS_H
#define _UTILS_H

/* other utility functions */

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


/* END other utility functions */


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


/* utility functions for matrix */

/* 
generate a square matrix with dimension and random-value elements
Params: int dimension, the rank/size/dimension of the matirx
Return: double**, pointer to the matrix
*/
double** genSquareMatrix(int dimension);

/* 
Generate a square matrix with dimension and all values 0
Params: int dimension, the rank/size/dimension of the matirx
Return: double**, pointer to the matrix
*/
double** gen0SquareMatrix(int dimension);

/* 
Print out a square matrix with dimension in a tabular way
Params: double** matrix: pointer to the matrix
		int row: number of rows
		int column: number of columns
Return: no return
*/
void printMatrix(double** matrix, int row, int column);

/* 
Compare two matrices and tell if they are equal. If one pair of elements are not equal,
print our error messages and return
Params: double** matrix1: pointer to the matrix 1
		double** matrix2: pointer to the matrix 2
		int row: number of rows
		int column: number of columns
Return: no return
*/
void compareMatrix(double** matrix1, double** matrix2, int row, int column);
/* END utility functions for matrix */


/* utility functions for parsing command line arguments */

/*
Get parameters from command line and parse them to integers
The behavior is same as atoi()
Params: argc, from main function
		argv, from main function
		matrix_size, pointer to a matrix_size integer
		num_threads, pointer to a num_threads integer
Return: 0: no change to matrix_size and num_threads
		-1: error happened, args are invalid or too many args
		1: values from the command are written to matrix_size and num_threads
*/
int parse_args(int argc, char* argv[], int* matrix_size,int* num_threads);
/* END utility functions for parsing command line arguments */

#endif /* _UTILS_H */