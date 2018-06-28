/*
FILE: main.c
Author: Yimou Cao
Date: 11/10/2017
Course: CSCI503
Lab: #5
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "utils.h"
#include "idmaker.h"
#include "people.h" // person struct and people queue struct
#include "elevator.h"
#include "gv.h" // struct of global variable

/* Wrapper functions for pthread */
/* Make a new thread which takes a function pointer and pointer to its global variable
Params: entry: function pointer. This is function is what the thread going to do
		gv: pointer to the global variable
Return: pthread_t, the thread handler
*/
pthread_t make_thread(void *(*entry)(void *), void* gv)
{
  int ret;
  pthread_t thread;

  ret = pthread_create(&thread, NULL, entry, gv);
  if (ret != 0) {
      perror_exit("pthread_create failed");
  }
  return thread;
}

/* Main thread waits for the thread which is passed in
Params: thread, the thread handler
Return: no return
*/
void join_thread(pthread_t thread)
{
  int ret = pthread_join(thread, NULL);
  if (ret != 0) {
      perror_exit("pthread_join failed");
  }
}
/* END Wrapper functions for pthread */

/*
Function for a thread which only do generating people every certain seconds,
whose from floor and to floor is randomly selected and from floor and to 
floor are not equal intrinsically. If time's up, it would stop generating 
automatically
Params: arg, pointer passed for accessing the global variable
Return: NULL. If any error happend in the process, the whole program would 
stop with some information.
*/
void* people_maker_entry(void* arg){
	if ( arg == NULL )
	{
		perror_exit("Invalid pointer passed in for people_maker thread");
	}
	// obtain global parameters
	GV* gv = (GV*) arg;
	int n_floors = gv->n_floors;
	int begining_time = gv->begining_time;
	int p = gv->people_speed; // the speed of generating people
	int simulation_time = gv->simulation_time;
	int* num_people_started = &(gv->num_people_started); // only people_maker thread would access this global
	PersonQueue* personqueue = gv->personqueue;
	IDMaker* person_id_maker = gv->person_id_maker;


	// start making people
	int status=0;
	while(time(0)-begining_time < simulation_time)
	{
		sleep(p); // sleep p seconds;
		int from = rand() % n_floors; // TODO: check errors 
		int to = -1; // TODO: check errors;  Is there any better way?
		do {
			to = rand() % n_floors;
		}while(to == from);

		unsigned long id = make_id(person_id_maker);
		double arrival_time = time(0); // TODO: get arrival time, time(0) return time_t
		Person* person = make_person(id, from, to, arrival_time);
		status = PersonQueue_add_person(personqueue, person);
		if(status <= 0){
			fprintf(stderr, "failed adding person to waiting queue\n");
			// TODO: exit or continue?
		} else {
			printf("[%f]: Person[%lu] arrives on floor %d, waiting to go to floor %d\n", 
				arrival_time-begining_time, id, from, to);
			(*num_people_started)++; // increment number of people started
		}

	}

    printf("[%f]: Stopping generating people\n", (double)time(0)-begining_time);
    return NULL;
}


/* Function for each elevator thread. One elevator only serve on person at a time. It picks
up one person from the global queue and transport him to the destination floor and then 
picks another and go on untill times up. 
Params: arg, pointer passed for accessing the global variable
Return: NULL. If any error happend in the process, the whole program would 
stop with some information.
*/
void* elevator_entry(void* arg){
	if ( arg == NULL )
	{
		perror_exit("Invalid pointer passed in for people_maker thread");
	}

	// obtain global parameters
	GV* gv = (GV*) arg;
	int n_floors = gv->n_floors;
	time_t begining_time = gv->begining_time;
	int e = gv->elevator_speed; // the speed of the elevator
	int simulation_time = gv->simulation_time;
	int* g_num_people_finished = &(gv->num_people_finished); // global number of people finished
	Mutex* n_finished_mutex = gv->n_finished_mutex;
	PersonQueue* personqueue = gv->personqueue;
	IDMaker* elevator_id_maker = gv->elevator_id_maker;

	//initialize an elevator
	unsigned long eid = make_id(elevator_id_maker);
	int l_num_people_finished = 0; // local number of people finished to this elevator
	Elevator* elevator = make_elevator(eid);
	short direction = 0; // 1: up; -1: down;

	// start working
	printf("starting elevator[%lu]\n", eid);
    int status = 0; // status for whether letin fail or not
    int dest_floor = -1;
	while(time(0)-begining_time < simulation_time){
		if ( elevator->people == NULL )
		{// there's no people in the elevator, need to know a new direction
			dest_floor = make_sure_direction(elevator, personqueue, n_floors, &direction);
			if ( dest_floor == -1 ) 
			{ // time's up
				continue;
			}
		}

		// moving to destination floor
		move_to_dest_floor(elevator, personqueue, e, begining_time, dest_floor);

		// drop people
		int n_dropped = drop_people(elevator, personqueue, begining_time);
		l_num_people_finished += n_dropped;

		// let in people and set the min dest_floor

		status = letin_people(elevator, personqueue, direction, &dest_floor, n_floors, begining_time);
        if ( status == -1 ) // cannot get person
        {
            continue;
        }

	}

    printf("Stoping elevator[%lu]\n", eid);
    PersonQueue_set_timesup(personqueue);
    cv_signal(personqueue->cv); //signal others to stop waiting

    update_global_num_people_finished(n_finished_mutex, g_num_people_finished, l_num_people_finished);
	return NULL;
}


int main(int argc, char *argv[])
{
	/*
	Input parameters:
	 number of elevators: > 0
	 number of floor: >0
	 how often people arrive: in seconds per person
	 evlevator speed: e seconds per floor
	 total time to simulate: s seconds
	 seed of the random function
	*/
	int n_elevators = 5;
	int n_floors = 10;
	int people_speed = 5;
	int elevator_speed = 1;
	int simulation_time = 30;
	unsigned seed = 233666;

	int status = parse_args(argc, argv, &n_elevators, &n_floors, &people_speed,
		&elevator_speed, &simulation_time, &seed);
	if ( status < 0 )
	{
		fprintf(stderr, "Incorrect number of args; Or Invalid arg value\n");
		exit(-1);
	}

    if (n_floors <= 1)
    {
        fprintf(stderr, 
            "Incorrect number of floors; It's not necessary to equip %d-floor building with elevators\n",
            n_floors);
        exit(-1);
    }

	int i=0;
	pthread_t people_maker;
	pthread_t elevators[n_elevators];
	GV* gv = make_gv(n_elevators, n_floors, people_speed, elevator_speed, 
		simulation_time, seed);
	seed_generator(gv);

	set_begin_time(gv);
	// printf("%f\n", (double)gv->begining_time);

	people_maker = make_thread(people_maker_entry, gv);

	for (i = 0; i < n_elevators; ++i)
	{
		elevators[i] = make_thread(elevator_entry, gv);
	}


    for (i = 0; i < n_elevators; ++i)
    {
        join_thread(elevators[i]);
    }
	join_thread(people_maker);

	printf("Simulation Result:\n");
	printf("%d people have started, %d people have finished during %lu seconds:\n", 
		gv->num_people_started,
		gv->num_people_finished,
		time(0)-gv->begining_time);
	return 0;

}