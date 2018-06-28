/** 
 * @file executor.c
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief implementation of command executors
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> //open and close files
#include "bool.h"

#include "executor.h"
#include "builtins.h"


/* exucute cmd, and exit. This is usually used with child process. 
 * @param: command_block* cmd, which is a struct pointer pointed to a command block 
 * @return: void
 * this function only execute one command for each call. The process is as below:
 * 1. check if it has file-in redirection. If it does do open file and duplicate
 * file descriptor 
 * 2. check if it has file-out redirection. If it does do open file and duplicate
 * file descriptor 
 * 3. check if it's a builtin command. If it is, execute builtin function by received
 * function pointer. If not, execute externally. */
void execute(command_block* cmd)
{
	int in_fd = -1;
	int out_fd = -1;
	if ( cmd->flag & IN_REDIRECTED )
	{
		in_fd = open(cmd->in_file, O_RDONLY);
		if ( in_fd == -1 )
		{
			fprintf(stderr, "[Shell ERROR] %s", cmd->in_file);
			perror(": open file error");
			exit(EXIT_FAILURE);
		}
		if (dup2(in_fd,0) < 0)
		{
			fprintf(stderr, "[Shell ERROR]");
			perror(": in redirect dup2 error");
			exit(EXIT_FAILURE);
		}
        close(in_fd);
	}
	if ( cmd->flag & OUT_REDIRECTED )
	{
		out_fd = open(cmd->out_file, O_CREAT|O_WRONLY|O_TRUNC, 0666);
		if ( out_fd == -1 )
		{
			fprintf(stderr, "[Shell ERROR] %s", cmd->out_file);
			perror(": open file error");
			exit(EXIT_FAILURE);
		}
		if (dup2(out_fd,1) < 0)
		{
			fprintf(stderr, "[Shell ERROR]");
			perror(": in redirect dup2 error");
			exit(EXIT_FAILURE);
		}
        close(out_fd);
	}
	if ( cmd->flag & OUT_REDIRECTED_APPEND )
	{
		out_fd = open(cmd->out_file, O_CREAT|O_WRONLY|O_APPEND, 0666);
		if ( out_fd == -1 )
		{
			fprintf(stderr, "[Shell ERROR] %s", cmd->out_file);
			perror(": open file error");
			exit(EXIT_FAILURE);
		}
		if (dup2(out_fd,1) < 0)
		{
			fprintf(stderr, "[Shell ERROR]");
			perror(": in redirect dup2 error");
			exit(EXIT_FAILURE);
		}
        close(out_fd);
	}

	char** args = cmd->args;
	builtin_func_ptr ptr = is_builtin(args[0]);
	if ( ptr )
	{
		if ((*ptr)(args) < 0)
		{
			fprintf(stderr, "[Shell ERROR] %s", args[0]);
			perror(": exec builtin command error");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if (execvp(args[0], args) < 0) 
		{
			fprintf(stderr, "[Shell ERROR] %s", args[0]);
		  	perror(": exec external command error");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);  /* guarantee builtin commands exit */
}


/* exucute commands by recursively calling this function
 * @param: command_block* cmd, which is a struct pointer pointed to a header command
 * block. By design, command blocks are connected through a linked list if they are 
 * piped together. 
 * @return: void
 * If there's only one command in the list, call execute() immediately. Otherwise create pipes
 * and duplicate them with stdin and stdout recursively.
 */
void do_pipe(command_block* cmd)
{
    if (cmd->next == NULL)
    {
        execute(cmd); // if only one cmd, execute and exit, no need to creat pipe
    }

    int fd[2];
    pipe(fd);  // 0 reading, 1 writing
    if (fork() == 0)
    {
    	/* for child process */
        if (dup2(fd[1], 1) != 1 )
        {
        	perror("[Shell ERROR]: pipe dup2 writing end");
        	exit(EXIT_FAILURE);
        }

        close(fd[0]);
        close(fd[1]);
        execute(cmd);
    }
    /* for parent process */
    if (dup2(fd[0], 0) != 0)
    {
    	perror("[Shell ERROR]: pipe dup2 reading end");
        exit(EXIT_FAILURE);
    }
    close(fd[0]);
    close(fd[1]);
    do_pipe(cmd->next);
}