/** 
 * @file main.c
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief global macros and declarations for this application
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "bool.h"

#include "parser.h"
#include "executor.h"
#include "builtins.h"
#include "main.h"


/* Implementation of a prompter.
 * @param: no parameters
 * @return: void
 * Simply print out the prompt string
*/
void prompt()
{
	#ifdef DEBUG
	printf("[DEBUG] ");
	#endif
	printf("UNIX> ");
}

/* Main function, entrance of the application */
int main(int argc, char const *argv[])
{
	int pid, status;
	bool bg_flag;
	char* line = NULL;
	command_block* cmd = NULL;

	while(1)
	{
		//checking background jobs
		if((pid = waitpid(-1, &status, WNOHANG)) > 0) {
	        printf("[Proc %d: exited with code %d]\n",
	               pid, WEXITSTATUS(status));
	    }

		//free some pointers and allocations and be ready to accept another line
		while(cmd)
		{
			command_block* to_free = cmd;
			cmd = cmd->next;
			free(to_free);
		}
		free(line);

		//set background execution flag to false
		bg_flag = false;

		#ifdef DEBUG
		printf("\n>>> 1. start prompt and waiting for user's input\n");
		#endif
		prompt();
		line = readline();

		#ifdef DEBUG
		printf(">>> 2. parse the line and packing tokens into commands\n");
		#endif
		cmd = parseline(line, &bg_flag);

		if (cmd == NULL)
		{
			//do nothing and continue to accept another line
			continue;
		}

		#ifdef DEBUG
    	printf(">>> 3. checkin if it's 'cd' command\n");
		#endif

		/* check if cd is the only one command in line. because cd chdir only change
		 current process's directory so this command must get executed in the main process
		 */
		if (cmd->next == NULL)
	    {
	    	char** args = cmd->args;
	        builtin_func_ptr chdir_ptr = is_chdir(args[0]);
	        if ( chdir_ptr )
			{
				if ((*chdir_ptr)(args) < 0)
				{
					perror("[Shell ERROR] exec cd command error");
				}

				// whether chdir successfully or not, continue to accept lines;
				continue;
			}
			// if it's not cd command, do nothing
	    }

	    #ifdef DEBUG
    	printf(">>> 4. fork a child. Using the child to execute commands with pipes\n");
		#endif

		pid = fork();
		if ( pid == 0 )
		{
			do_pipe(cmd);
			exit(EXIT_SUCCESS);
		}
		else if ( pid < 0 )
		{
			/* The fork failed.  */
          	perror ("fork");
          	continue;
		}
		
		// do background
		if ( bg_flag == true )
		{
			printf("[Proc %d: now in background]\n", pid);
		}
		else
		{
			wait(&status);
			if(WEXITSTATUS(status))
			{
				fprintf(stderr,	"Error occurred\n");
			}
		}
		
	}
	return 0;
}