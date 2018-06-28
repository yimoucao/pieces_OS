/** 
 * @file builtins.h
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief declaration of functions including builtin commands and utility functions
 */


#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin_func_ptr)(char** args);


/* Declaration of is_builtin().
 * @param: char* cmd, string of a command, option arguments are not included.
 * @return: builtin_func_ptr, if success this will return a corresponding function's pointer
 * else return NULL
*/
builtin_func_ptr is_builtin(char* cmd);


/* Declaration of is_chdir().
 * @param: char* cmd, string of a command, option arguments are not included.
 * @return: builtin_func_ptr, if success this will return the changing dir function's
 * pointer else return NULL
*/
builtin_func_ptr is_chdir(char* cmd);

#endif /* BUILTINS_H */