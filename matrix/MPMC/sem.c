// sem.c
// File of semaphore implementations
// Author: Yimou Cao
// Date: 10/29/2017
// Class: CSCI 503
// Lab 4


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils.h"
#include "sem.h"


/* Make a Semaphore struct. This includes allocating memory for it and initializing its value,
mutex and conditional variable.
Params: int value, the value of the semaphore
Return: the pointer to the Semaphore struct
*/
Semaphore* make_semaphore(int value)
{
  Semaphore* semaphore = malloc_check(sizeof(Semaphore)); // allocating memory
  semaphore->value = value; // initialize the value
  semaphore->wakeups = 0; // count availabe resources just released from other threads
  semaphore->mutex = make_mutex(); // mutex of the semaphore
  semaphore->cv = make_cv(); // conditional variable of the semaphore
  return semaphore;
}

/* P() operation on a semaphore. Decrease the value, put the thread into waiting state
if the value < 0 
Params: semaphore, the semaphore this operation is acted on 
Return: no return 
*/
void semaphore_P(Semaphore* semaphore)
{
  mutex_lock(semaphore->mutex);
  semaphore->value--;

  if (semaphore->value < 0) { // if value < 0, put the thread into wait/sleep state
    do {
      cv_wait(semaphore->cv, semaphore->mutex);
    } while (semaphore->wakeups < 1);
    semaphore->wakeups--;
  }
  mutex_unlock(semaphore->mutex);
}

/* V() operation on a semaphore. Increase the value, signal other threads if the
value <= 0. The availabe resource counter is increased before signaling
Params: semaphore, the semaphore this operation is acted on 
Return: no return 
*/
void semaphore_V(Semaphore* semaphore)
{
  mutex_lock(semaphore->mutex);
  semaphore->value++;

  if (semaphore->value <= 0) {
    semaphore->wakeups++; // update availabe resources just released from threads
    cv_signal(semaphore->cv); // signal other threads
  }
  mutex_unlock(semaphore->mutex);
}
