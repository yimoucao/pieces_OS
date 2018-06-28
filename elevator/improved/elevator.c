/*
FILE: elevator.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "elevator.h"

/* Make an elevator: assign the id for it and initialize its member variables 
Params: id, the id is going to be assigned to the new elevator
Return: the pointer to the newly created elevator struct
*/
Elevator* make_elevator(unsigned long id){
	Elevator* elevator = malloc_check(sizeof(Elevator));
	elevator->id = id;

	elevator->current_floor = 0;
	// TODO
	// elevator->lock = NULL;
	// elevator->cv = NULL;
	elevator->n_people = 0;
	elevator->people = NULL;

	return elevator;
}

/* Only for Basic Version functions */

/* Let one person into the elevator. Get one person in the queue and add it to the elevator 
Params: elevator, pointer to the elevator which is going to pick up one person
		queue, pointer to the people queue
Return: an integer indicating this operation is failed or successful. -1 means it gets no person
		1 means it does get one person
*/
int letin_person(Elevator* elevator, PersonQueue* queue){
	if ( elevator == NULL || queue == NULL )
	{
		perror_exit("Invalid parameters for the elevator to consume person");
	}
	Person* person = PersonQueue_pop_person(queue);
	if(person == NULL){
		if ( queue->timesup )
		{
			printf("Elevator[%lu] cannot get person because time's up\n", elevator->id);
			return -1; // no person, times up
		}else{
			fprintf(stderr, "Elevator[%lu] get a NULL person", elevator->id);
			exit(-1);
		}
	}
	elevator->people = person;
	elevator->n_people +=1;
	return 1; // have person
}


/* Simulating the process of transport the person in the elevator. It would sleep when changing
floors. Whne times up, return immediately with value 0, which indicating the person is not transported
successfully. Otherwiese return 1
Params: elevator, pointer to the elevator which is going to pick up one person
		queue, pointer to the people queue
		speed_per_floor, the seconds for the elevator thread to sleep per floor
		program_begin_time, the begining time of the simulator program
Return: an integer indicating how many people transferred during the process
*/
void transport_person(Elevator* elevator, int speed_per_floor, time_t program_begin_time){
	if ( elevator == NULL || speed_per_floor < 0 || program_begin_time < 0)
	{
		perror_exit("Invalid parameters for the elevator to consume person");
	}
	Person* person = elevator->people;
	if(person == NULL){
		fprintf(stderr, "Elevator[%lu] no person in it, unable to transport", elevator->id);
		exit(-1);
	}
	int from = person->from_floor;
	int to = person->to_floor;

	double cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] starts moving from floor[%d] to floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor, from);
	sleep(abs(from - elevator->current_floor)*speed_per_floor);

	elevator->current_floor = from;
	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] arrives at floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor);

	printf("[%f]: Elevator[%lu] picks up person[%lu]\n", 
		cur_time, elevator->id, person->id);

	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] starts moving from floor[%d] to floor[%d]\n", 
		cur_time, elevator->id, from, to);
	sleep(abs(to - from)*speed_per_floor);


	elevator->current_floor = to;
	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] arrives at floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor);

	printf("[%f]: Elevator[%lu] drops person[%lu]\n", 
		cur_time, elevator->id, person->id);

	elevator->people = NULL;
	elevator->n_people -=1;
	free(person);
}

/* update the number of people finished in the global variable
Params: n_finished_mutex, the mutex for the variable
		g_num_people_finished, the pointer to the variable
		value, the value need to be added
Return: no return
*/
void update_global_num_people_finished(Mutex* n_finished_mutex, int* g_num_people_finished, int value){
	if (n_finished_mutex == NULL || g_num_people_finished == NULL || value < 0)
	{
		perror_exit("Invalid parameters for the elevator to update global finished number");
	}

	mutex_lock(n_finished_mutex);
	*g_num_people_finished += value;
	mutex_unlock(n_finished_mutex);

	return;
}



//SECTION for improved version

/*
Make sure which direction a elevator is going to take when the elevator is empty
params: elevator: pointer to the elevator
		queue: pointer to the global people queue
		n_floors: number of floors
		short* direction: write 1: up; write -1: down
return: destination floor. -1: time's up; others: the immediate destination floor and 
	indicating successfully write direction
*/
int make_sure_direction(Elevator* elevator, PersonQueue* queue, int n_floors, short* direction){
	if ( elevator == NULL || queue == NULL || direction == NULL )
	{
		perror_exit("Invalid parameters for the elevator to consume person");
	}
	
	mutex_lock(queue->mutex);

	while (!queue->timesup && (queue->number <= 0 || queue->head == NULL) )
	{ // if no person in queue
		cv_wait(queue->cv, queue->mutex); // wait
	}

	if ( queue->timesup )
	{
		cv_signal(queue->cv); // awaken others
		mutex_unlock(queue->mutex);
		return -1; // time's up
	}
	
	Person* person = queue->head; // cursor to loop over the queue
	Person* nearest = queue->head;
	int dest_floor = -1;
	int distance = n_floors;
	while(person != NULL ){
		int tmp_distance = abs(elevator->current_floor - person->from_floor);
		if ( tmp_distance < distance )
		{
			distance = tmp_distance;
			nearest = person;
		}
		person = person->next;
	}

	if ( distance )
	{  // found no person on the current floor
		if ( nearest->from_floor > elevator->current_floor ) // no possiblity of equality because distance > 0
		{
			*direction = 1; // up
		}
		else{
			*direction = -1; // down
		}
		dest_floor = nearest->from_floor;
	}
	else 
	{ // distance==0, found people on the current floor
		if ( nearest->to_floor > elevator->current_floor )
		{
			*direction = 1; // up
		}else { // its not possible that to_floor = current_floor because current == from
			*direction = -1; //down   
		}
		dest_floor = elevator->current_floor;
	}
	
	mutex_unlock(queue->mutex);

	return dest_floor;
}

/* Move to the nearest floor people in the elevator want to
Params: elevator, pointer to the elevator
		queue, pointer to the global queue
		speed_per_floor, speed for the elevator to move by one floor
		program_begin_time, the time when the simulation program begins
		dest_floor, the nearest floor people in the elevator want to
Return: no return
*/
void move_to_dest_floor(Elevator* elevator, PersonQueue* queue, int speed_per_floor, time_t program_begin_time, int dest_floor){
	if ( elevator == NULL || speed_per_floor < 0 || program_begin_time < 0 || dest_floor < 0)
	{
		perror_exit("Invalid parameters for the elevator to move to dest_floor");
	}

	if ( queue->timesup )
	{
		// printf("[%f]: Elevator[%lu]\n", );
		return;
	}

	int from = elevator->current_floor;
	int to = dest_floor;

	double cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] starts moving from floor[%d] to floor[%d]\n", 
		cur_time, elevator->id, from, to);
	sleep(abs(from - to)*speed_per_floor);

	if ( queue->timesup )
	{
		return;
	}

	elevator->current_floor = to;
	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] arrives at floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor);
}

/*
When the elevator arrives at a certain floor, drop persons
Params: elevator, pointer to the elevator
		queue, pointer to the global people queue
		program_begin_time, the timet when the simulation program begins
return: number of people dropped
*/
int drop_people(Elevator* elevator, PersonQueue* queue, time_t program_begin_time){
	if ( elevator == NULL )
	{
		perror_exit("Invalid parameters for the elevator to drop people");
	}

	if ( elevator->people == NULL || elevator->n_people == 0 || queue->timesup )
	{
		return 0;
	}

	Person* person = elevator->people; // cursor to loop over people in the elevator
	Person* dropped = NULL;
	while( person != NULL ){
		Person* prev = person->prev;
		Person* next = person->next;
		if ( person->to_floor == elevator->current_floor)
		{
			if ( prev == NULL && next == NULL )
			{
				elevator->people = NULL;
			} else if (prev == NULL && next != NULL)
			{
				elevator->people = next;
				next->prev = NULL;
			} else if (prev != NULL && next == NULL)
			{
				prev->next = NULL;
			} else {
				prev->next = next;
				next->prev = prev;
			}
			// free(person);
			// put this person into dropped list
			person->prev = NULL;
			person->next = NULL;
			if ( dropped == NULL )
			{
				dropped = person;
			}else { // append to the head
				dropped->prev = person;
				person->next = dropped;
				dropped = person;
			}

			(elevator->n_people)--;
		}
		person = next;
	}

	// free dropped and print out
	double cur_time = 0;
	int n_dropped = 0;
	if ( dropped == NULL )
	{
		cur_time = time(0) - program_begin_time;
		printf("[%f]: Elevator[%lu]: No person dropped\n", cur_time, elevator->id);
	}
	else {
		Person* person = dropped;
		Person* cur_person = NULL; // person to be freed
		String* line = make_string("", 50);
		char buffer[48];
		while(person != NULL) {
			sprintf(buffer, "Person[%lu] ", person->id);
			string_concat(line, buffer); // put the Person[id] string into buffer
			cur_person = person;
			person = person->next;
			free(cur_person);
			n_dropped++;
		}
		cur_time = time(0) - program_begin_time;
		printf("[%f]: Elevator[%lu] dropped %s\n", cur_time, elevator->id, line->value);
	}

	return n_dropped;
}


/*
When the elevator arrives at a certain floor, let in some people with correct direction
Params: elevator, pointer to the elevator
		queue, pointer to the global people queue
		direction, up or down
		dest_floor, the pointer to nearest destination floor, write new value when let people in
return: number of people dropped
*/
int letin_people(Elevator* elevator, PersonQueue* queue, short direction, int* dest_floor, 
	int n_floors, time_t program_begin_time){
	if ( elevator == NULL || queue == NULL || dest_floor==NULL || direction==0)
	{
		perror_exit("[Improved] Invalid parameters for the elevator to consume people");
	}
	int num_get = 0;
	Person* person = PersonQueue_pop_people(queue, elevator->current_floor, direction, 
		&num_get);
	if(person == NULL && queue->timesup){
		printf("Elevator[%lu] cannot get person because time's up\n", elevator->id);
		return -1; // no person, times up
	}

	Person* cursor = NULL;
	int temp_dest_floor = -1;
	int distance = n_floors;

	cursor = elevator->people;
	while(cursor!=NULL){
		int temp_distance =  abs(cursor->to_floor - elevator->current_floor);
		if ( temp_distance < distance)
		{
			distance = temp_distance;
			temp_dest_floor = cursor->to_floor;
		}

		if ( cursor->next==NULL)
		{
			break;
		}
		cursor = cursor->next;
	}

	Person* prev = NULL; // last one of the people in the elevator
	if ( cursor==NULL) // no person in the elevator
	{
		elevator->people = person;
		// Person* prev = NULL;
		cursor = person;
	}else{
		cursor->next = person; //concate two double-linked list
		prev = cursor;
		cursor = cursor->next;
	}
	
	if ( person != NULL )
	{
		person->prev = prev;

		String* line = make_string("", 50);
		char buffer[48];
		while(cursor!=NULL){
			sprintf(buffer, "Person[%lu] ", cursor->id);
			string_concat(line, buffer); // put Person[id] string into buffer

			int temp_distance =  abs(cursor->to_floor - elevator->current_floor);
			if ( temp_distance < distance)
			{
				distance = temp_distance;
				temp_dest_floor = cursor->to_floor;
			}
			cursor = cursor->next;
		}
		double cur_time = time(0) - program_begin_time;
		printf("[%f]: Elevator[%lu] picked %s\n", cur_time, elevator->id, line->value);
	} else { // person == NULL 
		// do nothing
	}

	// elevator->people = person;
	*dest_floor = temp_dest_floor;
	elevator->n_people += num_get;
	return 1; // have person
}

// END SECTION for improved version