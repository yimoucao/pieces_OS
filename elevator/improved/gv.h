/*
FILE: gv.h
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/



#ifndef _GV_H
#define _GV_H

#include <time.h>
#include <pthread.h>
#include <idmaker.h>
#include <people.h>
#include "utils.h"

// define the type of the global variable
typedef struct {
	int n_elevators;
	int n_floors;
	time_t begining_time;
	int people_speed; // how many seconds per person
	int elevator_speed; // how many seconds per floor
	int simulation_time;
	unsigned random_seed;
	int num_people_started;
	int num_people_finished;
	Mutex* n_finished_mutex; // mutex for variable num_people_finished
	PersonQueue* personqueue; // pointer to the global people queue
	IDMaker* person_id_maker; // pointer to a id maker for newly created persons
	IDMaker* elevator_id_maker; // pointer to a id maker for newly created elevators
} GV;

/*
Make a new global varialbe according to the argumetns passed in.
Params: n_elevators, number of elevators
		n_floors, number of floors
		people_speed, the speed in seconds for people-generating thread to make new persons
		elevator_speed, the speed in seconds for elevater-thread to move by one floor
		simulation_time, the simulation time for the simulator program
		seed, the seed for the random number generator
Return: pointer to the newly created global variable struct
*/
GV* make_gv(int n_elevators, int n_floors, int people_speed, int elevator_speed, 
	int simulation_time, unsigned seed);

/*
Set the beginning time of the simulation in the global variable
Params: gv, the pointer to the global variable
Return: no return
*/
void set_begin_time(GV* gv);

/*
Seed the random number generator in the global variable
Params: gv, the pointer to the global variable
Return: no return
*/
void seed_generator(GV* gv);


#endif /* _GV_H */