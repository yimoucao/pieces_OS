/*
// utils.h
// Implementation of utility functions for the program
including error printing out, malloc and check, wrapper functions for mutex
and matrix generating and comparison
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "utils.h"



/* other utility functions */

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

/* END other utility functions */


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


/* utility functions for matrix */

// define the max value and the mininum value
#define MAX_VAL 100
#define MIN_VAL 0


/* 
generate a square matrix with dimension and random-value elements
Params: int dimension, the rank/size/dimension of the matirx
Return: double**, pointer to the matrix
*/
double** genSquareMatrix(int dimension){

	double** matrix = malloc_check(dimension * sizeof(double*));
	
	int i, j;
	for(i=0; i<dimension; i++){
		matrix[i] = malloc_check(dimension * sizeof(double));
	}

	//set up seed of random generator
	srandom(time(0)+clock()+random());

	for(i=0; i<dimension; i++){
		for(j=0; j<dimension; j++){
			matrix[i][j] = rand() % MAX_VAL + MIN_VAL;
		}
	}

	return matrix;
}

/* 
Generate a square matrix with dimension and all values 0
Params: int dimension, the rank/size/dimension of the matirx
Return: double**, pointer to the matrix
*/
double** gen0SquareMatrix(int dimension){

	double** matrix = malloc_check(dimension * sizeof(double*));

	int i, j;
	for(i=0; i<dimension; i++){
		matrix[i] = malloc_check(dimension * sizeof(double));
	}

	for(i=0; i<dimension; i++){
		for(j=0; j<dimension; j++){
			matrix[i][j] = 0;
		}
	}

	return matrix;
}


/* 
Print out a square matrix with dimension in a tabular way
Params: double** matrix: pointer to the matrix
		int row: number of rows
		int column: number of columns
Return: no return
*/
void printMatrix(double** matrix, int row, int column){
	int i, j;

	for(i = 0; i < row; i++) {
      for(j = 0; j < column; j++) {
         printf("%10.3f\t", matrix[i][j]);
      }
      printf("\n");
   }
}


/* 
Compare two matrices and tell if they are equal. If one pair of elements are not equal,
print our error messages and return
Params: double** matrix1: pointer to the matrix 1
		double** matrix2: pointer to the matrix 2
		int row: number of rows
		int column: number of columns
Return: no return
*/
void compareMatrix(double** matrix1, double** matrix2, int row, int column){
	int i, j;

	for(i = 0; i < row; i++) {
      for(j = 0; j < column; j++) {
         if ( matrix1[i][j] != matrix2[i][j])
         {
         	fprintf(stderr, "NOT EQUAL! (At least one from [%d, %d])\n", i+1, j+1);
         	return;
         }
      }
    }
    printf("SUCCESS\n");
}
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
int parse_args(int argc, char* argv[], int* matrix_size,int* num_threads)
{
	if (argc == 1)
	{
		return 0; // use default args
	}
	else if (argc == 3)
	{
		int a,b;
		a = atoi(argv[1]); // conver string to integer
		b = atoi(argv[2]); // convert string to integer

		if (a <= 0 || b <= 0)
		{
			return -1; // invalid args
		}

		*matrix_size = a;
		*num_threads = b;

		return 1; //values from the command are written to matrix_size and num_threads
	}
	else
	{
		return -1; // Incorrect number of args
	}
}


/* END utility functions for parsing command line arguments */