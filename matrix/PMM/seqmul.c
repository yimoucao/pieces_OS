/*
// seqmul.c
// Implementations of functions for sequential multiplication of two matrix
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/


#include <stdlib.h>
#include "seqmul.h"

/*
Sequential multiplication for matrices. Through 3 for-loops to calculate each sum of a certai row and column
Params: double** matrixA: pointer to the matrix A
		double** matrixB: pointer to the matrix B
		double** matrixC: pointer to the matrix C, the result matrix
		int dimension: rank/size of these matrices
Return: -1: error happened like a parameter is not valid
		1: the whole calculation process is successful
*/
int sequentialMultiply(double** matrixA, double** matrixB, double** matrixC, int dimension){
	if ( matrixA == NULL || matrixB == NULL || matrixC == NULL || dimension < 1)
	{
		return -1; //failure
	}


	int i, j, k;

	for(i=0; i<dimension; i++){
		for(j=0; j<dimension; j++){
			for(k=0; k<dimension; k++){
				matrixC[i][j] += matrixA[i][k] * matrixB[k][j]; // sum up all
			}
		}
	}

	return 1; //success

}