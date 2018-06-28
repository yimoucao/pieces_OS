/*
FILE: elevator.h
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/


#ifndef _ELEVATOR_H
#define _ELEVATOR_H

#include <time.h>
#include <pthread.h>
#include "people.h"
#include "utils.h"

// define a new type of the elevator
typedef struct {
	unsigned long id;
	int current_floor;
	pthread_mutex_t lock;
	pthread_cond_t cv;
	int n_people; // count how many people in the elevator
	Person* people; // the pointer to the head of a double-linked list persons
} Elevator;


/* Make an elevator: assign the id for it and initialize its member variables 
Params: id, the id is going to be assigned to the new elevator
Return: the pointer to the newly created elevator struct
*/
Elevator* make_elevator(unsigned long id);


/* Only for Basic Version functions */

/* Let one person into the elevator. Get one person in the queue and add it to the elevator 
Params: elevator, pointer to the elevator which is going to pick up one person
		queue, pointer to the people queue
Return: an integer indicating this operation is failed or successful. -1 means it gets no person
		1 means it does get one person
*/
int letin_person(Elevator* elevator, PersonQueue* queue); // let one person into the elevator

/* Simulating the process of transport the person in the elevator. It would sleep when changing
floors. Whne times up, return immediately with value 0, which indicating the person is not transported
successfully. Otherwiese return 1
Params: elevator, pointer to the elevator which is going to pick up one person
		queue, pointer to the people queue
		speed_per_floor, the seconds for the elevator thread to sleep per floor
		program_begin_time, the begining time of the simulator program
Return: an integer indicating how many people transferred during the process
*/
void transport_person(Elevator* elevator, int speed_per_floor, time_t program_begin_time);

/* END Only for Basic Version functions */


/* update the number of people finished in the global variable
Params: n_finished_mutex, the mutex for the variable
		g_num_people_finished, the pointer to the variable
		value, the value need to be added
Return: no return
*/
void update_global_num_people_finished(Mutex* n_finished_mutex, int* g_num_people_finished, int value);


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
int make_sure_direction(Elevator* elevator, PersonQueue* queue,int n_floors, short* direction);

/* Move to the nearest floor people in the elevator want to
Params: elevator, pointer to the elevator
		queue, pointer to the global queue
		speed_per_floor, speed for the elevator to move by one floor
		program_begin_time, the time when the simulation program begins
		dest_floor, the nearest floor people in the elevator want to
Return: no return
*/
void move_to_dest_floor(Elevator* elevator, PersonQueue* queue, int speed_per_floor, time_t program_begin_time, int dest_floor);


/*
When the elevator arrives at a certain floor, drop persons
Params: elevator, pointer to the elevator
		queue, pointer to the global people queue
		program_begin_time, the timet when the simulation program begins
return: number of people dropped
*/
int drop_people(Elevator* elevator, PersonQueue* queue, time_t program_begin_time);

/*
When the elevator arrives at a certain floor, let in some people with correct direction
Params: elevator, pointer to the elevator
		queue, pointer to the global people queue
		direction, up or down
		dest_floor, the pointer to nearest destination floor, write new value when let people in
return: number of people dropped
*/
int letin_people(Elevator* elevator, PersonQueue* queue, short direction, int* dest_floor, 
	int n_floors, time_t program_begin_time);


#endif /* _ELEVATOR_H */