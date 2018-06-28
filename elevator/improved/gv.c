/*
FILE: gv.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/


#include "gv.h"


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
	int simulation_time, unsigned seed){
	GV* gv = malloc_check(sizeof(GV));
	gv->n_elevators = n_elevators;
	gv->n_floors = n_floors;
	gv->begining_time = 0;
	gv->people_speed = people_speed;
	gv->elevator_speed = elevator_speed;
	gv->simulation_time = simulation_time;
	gv->random_seed = seed;
	gv->num_people_started = 0; // number of people added to queue
	gv->num_people_finished = 0;
	gv->n_finished_mutex = make_mutex(); // Mutex for num_people_finished
	gv->personqueue = PersonQueue_make_queue();
	gv->person_id_maker = make_IDMaker(); // create a new id maker for persons
	gv->elevator_id_maker = make_IDMaker(); // create a new id maker for elevators
	return gv;
}

/*
Set the beginning time of the simulation in the global variable
Params: gv, the pointer to the global variable
Return: no return
*/
void set_begin_time(GV* gv){
	if ( gv == NULL )
	{
		perror_exit("Invalid GV pointer when setting beginning time.");
	}
	gv->begining_time = time(0);
}

/*
Seed the random number generator in the global variable
Params: gv, the pointer to the global variable
Return: no return
*/
void seed_generator(GV* gv){
	if ( gv == NULL )
	{
		perror_exit("Invalid GV pointer when feeding seed.");
	}
	srand(gv->random_seed);
}