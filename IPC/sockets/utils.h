/** 
 * @file utils.h
 * @author Yimou Cao
 * @date 10/09/2017
 * Course: CSCI503
 * Lab: 3
 */

#ifndef UTILS_H
#define UTILS_H

#define MILLION_MAX (100)

/* parse the command line args 
  Return: int: status code, success 0; fail -1 
  Params: argc, argv: original params from main function, they are comman line args
          unsigned int: pointer to a int, used to store the parsed number which indicates
          that how many million messages user wants to send 
  Berief: firstly make sure there're only 1 or 3 parameters. If it's 1, user doesn't specify
          how many. If it's 3, furtherly check if the option is -m and then parse the number
          after it. and the number of millions are limited to [1, MILLION_MAX]*/
int parse_args(int argc, char *argv[], unsigned int * x_million);


/* get current time
  Return: double: seconds since the Epoch 
  Params: none
  Brief: gettimeofday() fetch time current time since Epoch and save it to tv
         and tz. tv.tv_sec is the whole seconds and tv.tv_usec is the rest of
         the time, represented as the number of microsenconds 
*/
double get_cur_time();

#endif /* UTILS_H */