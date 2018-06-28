/** 
 * @file executor.h
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief global macros and declarations for this application
 */
//executor.h

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/* exucute commands by recursively calling this function
 * @param: command_block* cmd, which is a struct pointer pointed to a header command
 * block. By design, command blocks are connected through a linked list if they are 
 * piped together. 
 * @return: void
 * If there's only one command in the list, call execute() immediately. Otherwise create pipes
 * and duplicate them with stdin and stdout recursively.
 */
void do_pipe(command_block* cmd);


/* exucute cmd, and exit. This is usually used with child process. 
 * @param: command_block* cmd, which is a struct pointer pointed to a command block 
 * @return: void
 * this function only execute one command for each call.
 */
void execute(command_block* cmd);


#endif /* EXECUTOR_H */