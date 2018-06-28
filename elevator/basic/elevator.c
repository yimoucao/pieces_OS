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
int transport_person(Elevator* elevator, PersonQueue* queue, int speed_per_floor, time_t program_begin_time){
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
	if ( queue->timesup ){
		printf("[%f]: Elevator[%lu] cannot starts moving due to timesup\n", 
		cur_time, elevator->id);
		return 0;
	}
	sleep(abs(from - elevator->current_floor)*speed_per_floor);


	elevator->current_floor = from;
	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] arrives at floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor);

	if ( queue->timesup ){
		printf("[%f]: Elevator[%lu] cannot pick up person due to timesup\n", 
		cur_time, elevator->id);
		return 0;
	}

	printf("[%f]: Elevator[%lu] picks up person[%lu]\n", 
		cur_time, elevator->id, person->id);

	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] starts moving from floor[%d] to floor[%d]\n", 
		cur_time, elevator->id, from, to);
	if ( queue->timesup ){
		printf("[%f]: Elevator[%lu] cannot starts moving due to timesup\n", 
		cur_time, elevator->id);
		return 0;
	}
	sleep(abs(to - from)*speed_per_floor);

	if ( queue->timesup ){return 0;}

	elevator->current_floor = to;
	cur_time = time(0) - program_begin_time;
	printf("[%f]: Elevator[%lu] arrives at floor[%d]\n", 
		cur_time, elevator->id, elevator->current_floor);

	printf("[%f]: Elevator[%lu] drops person[%lu]\n", 
		cur_time, elevator->id, person->id);

	elevator->people = NULL;
	elevator->n_people -=1;
	free(person);

	return 1;
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