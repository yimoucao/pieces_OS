/*
FILE: people.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
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


/* Set time's up flag 
Params: queue, pointer to the queue it's going to set timesup
Return: no return
*/
void PersonQueue_set_timesup(PersonQueue* queue){
	if ( queue == NULL )
	{
		perror_exit("Invalid parameter for set timesup in queue");
	}
	queue->timesup = 1;
}


/* Clear time's up flag 
Params: queue, pointer to the queue it's going to clear timesup
Return: no return
*/
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
