/*
FILE: idmaker.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/


#include "utils.h"
#include "idmaker.h"


/* Make a new ID Maker 
Params: no input params
Return: pointer to the newly created id maker
*/
IDMaker* make_IDMaker(){
	IDMaker* id_maker = malloc_check(sizeof(IDMaker));
	id_maker->id = 0;
	id_maker->mutex = make_mutex();
	return id_maker;
}

/* Obtain a new id from the id maker 
Params: pointer to a id maker
Return: id
*/
unsigned long make_id(IDMaker* id_maker){
	mutex_lock(id_maker->mutex);
	unsigned long id = id_maker->id;
	(id_maker->id)++;
	mutex_unlock(id_maker->mutex);
	return id;
}