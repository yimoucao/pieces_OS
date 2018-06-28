/** 
 * @file utils.c
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>

#include "utils.h"

/* parse the command line args 
  Return: int: status code, success 0; fail -1 
  Params: argc, argv: original params from main function, they are comman line args
          unsigned int: pointer to a int, used to store the parsed number which indicates
          that how many million messages user wants to send 
  Berief: firstly make sure there're only 1 or 3 parameters. If it's 1, user doesn't specify
          how many. If it's 3, furtherly check if the option is -m and then parse the number
          after it. and the number of millions are limited to [1, MILLION_MAX]*/
int parse_args(int argc, char *argv[], unsigned int * x_million){
    if ( argc > 3 || argc%2 == 0) // make sure counts is a odd number
    {
        printf("Incorrect arg numbers\n");
        return -1;
    } else if ( argc > 1)
    {
        // becuase argc is odd, then argc must be 3
        int x_compare = strcmp(argv[1], "-m");

        if ( argv[1] && x_compare==0 )
        {
            unsigned int x = atoi(argv[2]); // becuase argc is odd, there must exists argv[2]
            if ( x < 1 || x > MILLION_MAX )
            {printf("Error: argument for -m invalid. Only accept number in [1, 99]\n");return -1;}

            *x_million = x;
            
        }else { printf("Invalid arguments\n");return -1;}
    }
    else {
        printf("The program would load default settings\n");
    }
    
    return 0; // success: 0, fail: -1
}


/* get current time
  Return: double: seconds since the Epoch 
  Params: none
  Brief: gettimeofday() fetch time current time since Epoch and save it to tv
         and tz. tv.tv_sec is the whole seconds and tv.tv_usec is the rest of
         the time, represented as the number of microsenconds 
*/
double get_cur_time() {
    struct timeval tv;
    struct timezone tz;
    double cur_time;

    if (gettimeofday(&tv, &tz) < 0)
    {
        printf("Error: failure to get time\n");
        exit(EXIT_FAILURE);
    }
    cur_time = tv.tv_sec + tv.tv_usec / 1000000.0;

    return cur_time;
}