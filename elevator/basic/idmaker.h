/*
FILE: idmaker.h
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/


#ifndef _IDMAKER_H
#define _IDMAKER_H

#include "utils.h"

/* Define a new type of ID Maker*/
typedef struct {
	unsigned long id;
	Mutex* mutex; // mutex to protext the id-making process
} IDMaker;

/* Make a new ID Maker 
Params: no input params
Return: pointer to the newly created id maker
*/
IDMaker* make_IDMaker();

/* Obtain a new id from the id maker 
Params: pointer to a id maker
Return: id
*/
unsigned long make_id(IDMaker* id_maker);

#endif /* _IDMAKER_H */