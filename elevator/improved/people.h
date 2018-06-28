/*
FILE: people.h
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
Description: Data types and methods about person, and people queue
*/

#ifndef _PEOPLE_H
#define _PEOPLE_H

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "sem.h"

/* Define a new type of Person */
typedef struct Person{
	unsigned long id;
	int from_floor;
	int to_floor;
	double arrival_time;
	struct Person* next;
	struct Person* prev;
} Person;

/* Make a new person according to the parameters passed int
Params: id, id obtained from id maker for this person
		from_floor, which floor the person is from
		to_floor, which floor the person is going
		arrival_time, time when the person is created
Return: pointer to the newly created person
*/
Person* make_person(unsigned long id, int from_floor, int to_floor, double arrival_time);


/* Define a new type of queue, which maintains newly generated persons but not picked up 
by any elevator yet.
*/
typedef struct {
	int number; // how many people are waiting
	Person* head; // head of the double-linked list
	Person* tail; // tail of the double-linked list
	Mutex* mutex;
	CV* cv;
	short timesup; // indicating if the simulation time is up
} PersonQueue;


/* Make a queue, which manages waiting people
Params: no parameters
Return: pointer to the queue
*/
PersonQueue* PersonQueue_make_queue();


/* Add one person to the queue
Params: queue, pointer to the queue
		person, pointer to the person who is going to be added to the queue
Return: status code, -1 means error. 1 means success
*/
int PersonQueue_add_person(PersonQueue* queue, Person* person);

/* Pop out one person for a elevator to pick up
Params: queue, pointer to the queue
Return: pointer to the popped person, NULL means failure
*/
Person* PersonQueue_pop_person(PersonQueue* queue); // pop one person at the begining of the double linked list


/* Set time's up flag 
Params: queue, pointer to the queue it's going to set timesup
Return: no return
*/
void PersonQueue_set_timesup(PersonQueue* queue);

/* Clear time's up flag 
Params: queue, pointer to the queue it's going to clear timesup
Return: no return
*/
void PersonQueue_clr_timesup(PersonQueue* queue);


// for improved version
/* Pop a bunch of peope who is at current floor from the globa queue into elevator 
Params: queue, pointer to the queue
		current_floor, the floor an elevator is currently at
		direction, the direction an elevator is taking
		number: how many people popped
return: a double-linked list of persons
*/
Person* PersonQueue_pop_people(PersonQueue* queue, int current_floor, short direction, int* number);

#endif /* _PEOPLE_H */