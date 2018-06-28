// sem.h
// Header file of semaphore
// Author: Yimou Cao
// Date: 10/29/2017
// Class: CSCI 503
// Lab 4

#ifndef SEM_H
#define SEM_H


/* value is the value of the semaphore. wakeups is the number of threads which are woken
but not got execution */
typedef struct {
	int value, wakeups;
	Mutex* mutex;
	CV* cv;
} Semaphore;

/* Make a Semaphore struct. This includes allocating memory for it and initializing its value,
mutex and conditional variable.
Params: int value, the value of the semaphore
Return: the pointer to the Semaphore struct
*/
Semaphore* make_semaphore(int value);

/* P() operation on a semaphore. Decrease the value, put the thread into waiting state
if the value < 0 
Params: semaphore, the semaphore this operation is acted on 
Return: no return 
*/
void semaphore_P(Semaphore *semaphore); //wait

/* V() operation on a semaphore. Increase the value, signal other threads if the
value <= 0. The availabe resource counter is increased before signaling
Params: semaphore, the semaphore this operation is acted on 
Return: no return 
*/
void semaphore_V(Semaphore *semaphore); //signal

#endif /* SEM_H */