/*
FILE: people.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
Description: Data types and methods about person, and people queue
*/


#include "people.h"

/* Make a new person according to the parameters passed int
Params: id, id obtained from id maker for this person
		from_floor, which floor the person is from
		to_floor, which floor the person is going
		arrival_time, time when the person is created
Return: pointer to the newly created person
*/
Person* make_person(unsigned long id, int from_floor, int to_floor, double arrival_time){
	Person* person = malloc_check(sizeof(Person));
	person->id = id;
	person->from_floor = from_floor;
	person->to_floor = to_floor;
	person->arrival_time = arrival_time;

	person->next = NULL;
	person->prev = NULL;
	return person;
}

/* Make a queue, which manages waiting people
Params: no parameters
Return: pointer to the queue
*/
PersonQueue* PersonQueue_make_queue(){
	PersonQueue* queue = malloc_check(sizeof(PersonQueue));
	queue->number = 0;
	queue->head = NULL;
	queue->tail = NULL;
	queue->mutex = make_mutex();
	queue->cv = make_cv();
	queue->timesup = 0; // set it to timesup = false
	return queue;
}

void PersonQueue_set_timesup(PersonQueue* queue){
	if ( queue == NULL )
	{
		perror_exit("Invalid parameter for set timesup in queue");
	}
	queue->timesup = 1;
}

void PersonQueue_clr_timesup(PersonQueue* queue){
	if ( queue == NULL )
	{
		perror_exit("Invalid parameter for clr timesup in queue");
	}
	queue->timesup = 0;
}

/* Add one person to the queue
Params: queue, pointer to the queue
		person, pointer to the person who is going to be added to the queue
Return: status code, -1 means error. 1 means success
*/
int PersonQueue_add_person(PersonQueue* queue, Person* person){
	if ( queue == NULL || person == NULL )
	{
		//TODO: fprintf error, or perror exit?
		return -1;
	}

	mutex_lock(queue->mutex);
	if ( queue->head == NULL || queue->number <= 0)
	{ //if no person in the queue
		queue->head = person;
		queue->tail = person;
		queue->number = 1;
	}else {
		queue->tail->next = person;
		person->prev = queue->tail;
		queue->tail = person;
		(queue->number)++;
	}
	cv_signal(queue->cv);
	mutex_unlock(queue->mutex);
	return 1;
}

// for basic version

/* Pop out one person for a elevator to pick up
Params: queue, pointer to the queue
Return: pointer to the popped person, NULL means failure
*/
Person* PersonQueue_pop_person(PersonQueue* queue){
	if ( queue == NULL )
	{
		perror_exit("Invalid parameter for pop a person in queue");
	}

	mutex_lock(queue->mutex);

	while (!queue->timesup && (queue->number <= 0 || queue->head == NULL) )
	{ // if no person in queue
		cv_wait(queue->cv, queue->mutex);
	}

	if ( queue->timesup )
	{
		cv_signal(queue->cv); // awaken others
		mutex_unlock(queue->mutex);
		return NULL;
	}
	
	Person* person = queue->head;
	if (queue->number == 1)
	{
		queue->head = NULL;
		queue->tail = NULL;
	}else {
		person->next->prev = NULL;
		queue->head = person->next;

		person->prev = NULL;
		person->next = NULL;
	}
	(queue->number)--;
	mutex_unlock(queue->mutex);

	return person;
}



// for improved version
/* Pop a bunch of peope who is at current floor from the globa queue into elevator 
Params: queue, pointer to the queue
		current_floor, the floor an elevator is currently at
		direction, the direction an elevator is taking
		number: how many people popped
return: a double-linked list of persons
*/
Person* PersonQueue_pop_people(PersonQueue* queue, int current_floor, short direction, int* number){
	if ( queue == NULL || current_floor<0 || direction==0 || number==NULL)
	{
		perror_exit("[improved] Invalid parameter for pop people in queue");
	}

	mutex_lock(queue->mutex);

	while (!queue->timesup && (queue->number <= 0 || queue->head == NULL) )
	{ // if no person in queue
		cv_wait(queue->cv, queue->mutex);
	}

	if ( queue->timesup )
	{
		cv_signal(queue->cv); // awaken others
		mutex_unlock(queue->mutex);
		return NULL;
	}
	
	Person* person = queue->head;
	Person* result = NULL;

	while(person != NULL ){
		Person* prev = person->prev;
		Person* next = person->next;
		if ( person->from_floor == current_floor)
		{
			if ( ((long)person->to_floor - person->from_floor) * direction > 0)
			{
				if ( prev == NULL && next == NULL )
				{
					queue->head = NULL;
					queue->tail = NULL;
				} else if (prev == NULL && next != NULL)
				{
					queue->head = next;
					next->prev = NULL;
				} else if (prev != NULL && next == NULL)
				{
					queue->tail = prev;
					prev->next = NULL;
				} else {
					prev->next = next;
					next->prev = prev;
				}

				// put this person into result list
				person->prev = NULL;
				person->next = NULL;
				if ( result == NULL )
				{
					result = person;
				}else { // append to the head
					result->prev = person;
					person->next = result;
					result = person;
				}
				(*number)++; // increase the number of picked persons

				(queue->number)--;
			}
		}

		person = next;
	}
	
	mutex_unlock(queue->mutex);

	return result;
}
