/* main.c
Main entry of the program
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "seqmul.h"
#include "parmul.h"


// define default parameters
#define MATRIX_SIZE 2
#define N_THREADS 2

/*main, entrance function
we only consider square matrices in the program
*/
int main(int argc, char *argv[])
{
	int dim = MATRIX_SIZE; // set the matrix size, which is also named dimension/rank
	int n_threads = N_THREADS; // set the number of threads, which are workers to get task and compute elements
	int status = parse_args(argc, argv, &dim, &n_threads); // parse and obtain the arguments obtained from command line
	if ( status < 0) // 0 no changes to variables; negative: error; postive: changed variales;
	{
		fprintf(stderr, "Incorrect number of args; Or Invalid arg values\n");
		exit(-1);
	}


	double** matrixA = genSquareMatrix(dim); //generate a square matrix radomsly
	double** matrixB = genSquareMatrix(dim); //generate a square matrix radomsly
	double** matrixResult1 = gen0SquareMatrix(dim); //generate a square matrix with all 0s
	double** matrixResult2 = gen0SquareMatrix(dim); //generate a square matrix with all 0s

	printf(">Matrix A:\n");
	printMatrix(matrixA, dim, dim);printf("\n");
	printf(">Matrix B:\n");
	printMatrix(matrixB, dim, dim);printf("\n");

	printf(">Sequential Multiplication:\n");
	if ( !sequentialMultiply(matrixA, matrixB, matrixResult1, dim))
	{
		fprintf(stderr, "Sequential Multiplication Failed.\n");
		exit(-1);
	}
	printMatrix(matrixResult1, dim, dim);printf("\n");
	
	printf(">Parallel Multiplication:\n");
	parallelMultiply(matrixA, matrixB, matrixResult2, dim, n_threads);
	printMatrix(matrixResult2, dim, dim);printf("\n");
	
	printf(">Comparison Result:\n");
	compareMatrix(matrixResult1, matrixResult2, dim, dim);

	free(matrixA);free(matrixB);free(matrixResult1);free(matrixResult2); //free all

	return 0;
}