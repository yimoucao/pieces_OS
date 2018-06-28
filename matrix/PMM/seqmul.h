/*
// seqmul.h
// functions for sequential multiplication of two matrix
Author: Yimou Cao
Date: 11/5/2017
Course: CSCI 503
Lab: #4
*/


#ifndef _SEQMUL_H
#define _SEQMUL_H

/*
Sequential multiplication for matrices. Through 3 for-loops to calculate each sum of a certai row and column
Params: double** matrixA: pointer to the matrix A
		double** matrixB: pointer to the matrix B
		double** matrixC: pointer to the matrix C, the result matrix
		int dimension: rank/size of these matrices
Return: -1: error happened like a parameter is not valid
		1: the whole calculation process is successful
*/
int sequentialMultiply(double** matrixA, double** matrixB, double** matrixC, int dimension);

#endif /* _SEQMUL_H */