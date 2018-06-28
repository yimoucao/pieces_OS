/** 
 * @file builtins.c
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief implementation of functions including builtin commands and utility functions
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "builtins.h"



/* Implementation of help command.
 * @param: char** args, not used in this function. It's placed here for the reason that
 * the function pointer can be returned to its invoker and the type of this kind function
 * pointer is fixed, see type definition in the builtins.h
 * @return: int, 
 * Simply print out some helpful information
*/
int builtin_help(char** args)
{
	printf("Here help comes!\n");
	printf("Builtin functions:\n");
	printf("cd\tchange working directory\nhelp\tshow some help\nabout\twhere credits go\n");
	return 0; //if success return 0
}


/* Implementation of about command.
 * @param: char** args, not used in this function. It's placed here for the reason that
 * the function pointer can be returned to its invoker and the type of this kind function
 * pointer is fixed, see type definition in the builtins.h
 * @return: int, 
 * Simply print out info about this application
*/
int builtin_about(char** args)
{
	printf("***************\n  [About]\n* Simple Shell\n* by Yimou Cao\n***************\n");
	return 0; //if success return 0
}


/* Implementation of cd command.
 * @param: char** args, args[0] is "cd", args[1] is path, ignore others as linux bash does.
 * @return: int, same as chidir(), return 0 if success
 * Chage current working directory by call chdir()
*/
int builtin_chdir(char** args)
{
	
	#ifdef DEBUG
	printf("chaging directory to %s\n", args[1]);
	#endif

	/* chdir(), Upon successful completion, 0 shall be returned. 
	* Otherwise, -1 shall be returned, the current working directory shall remain unchanged, 
	* and errno shall be set to indicate the error.
	* The chdir() function only affects the working directory of the current process. For 
	* this reason, this must be executed in the main process of the shell.
	*/
	return chdir(args[1]);
}


/* Implementation of is_builtin().
 * @param: char* cmd, string of a command, option arguments are not included.
 * @return: builtin_func_ptr, if success this will return a corresponding function's pointer
 * else return NULL
 * By string comparing with all predefined commands to see if it's a builtin command.
*/
builtin_func_ptr is_builtin(char* cmd)
{
	/* fisrtly test if it's empty */
	if (cmd)
	{
		if (strcmp(cmd,"help")==0)
		{
			return &builtin_help; /* return help function's pointer */
		}
		if (strcmp(cmd,"about")==0)
		{
			return &builtin_about; /* return about function's pointer */
		}
		if (strcmp(cmd,"cd")==0)
		{
			return &builtin_chdir; /* return cd function's pointer */
		}
	}
	return NULL;
}


/* Implementation of is_chdir().
 * @param: char* cmd, string of a command, option arguments are not included.
 * @return: builtin_func_ptr, if success this will return the changing dir function's
 * pointer else return NULL
 * By string comparing with 'cd' to see if it's changing directory command.
*/
builtin_func_ptr is_chdir(char* cmd)
{
	/* fisrtly test if it's empty */
	if ( cmd )
	{
		if ( strcmp(cmd, "cd") == 0 )
		{
			return &builtin_chdir; /* return cd function's pointer */
		}
	}
	return NULL;
}