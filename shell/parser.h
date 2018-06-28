/** 
 * @file parser.h
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief parse a line from inputs
 */
#ifndef PARSER_H
#define PARSER_H


#include "bool.h"
// incremental size of characters for one line
#define NUM_CHARS 2048

// incremental size of tokens for one line
#define TOKEN_BUF_SIZE 32
#define TOKEN_DELIM " \t\r\n\a"

// some flags about commands
#define IN_REDIRECTED 0x1
#define OUT_REDIRECTED 0x2
#define PIPIED 0x4
#define OUT_REDIRECTED_APPEND 0x8

// incremental size of arguments for one command (including command name)
#define ARGS_BUF_SIZE 32

// command block struct
struct command_block
{
	char** args; // args list
	unsigned int args_buf_size; // current buffer size of the args list
	unsigned int num_args; // number of args in the args list
	unsigned int flag;
	char* in_file;
	char* out_file;
	struct command_block* next;
};

typedef struct command_block command_block;


/* Read a line from user
 * @param: no param
 * @return char*, pointer to one line string
 * Through while loop it continuously get characters one by one until EOF or line feed is
 * received.
 */
char* readline();


/* Parse a line.
 * @param: char* line, a line of characters
 * @param: bool* bg_flag, pointer to a bool, to be written with backround flag
 * @return command_block*, the header command block of the linked list
 * The brief steps are as following
 * 1. tokenize
 * 2. commandize
 * 3. if the application is compiled with debug option, print out commands and args
 */
command_block* parseline(char* line, bool* bg_flag);


/* Push one token into the command block's args attribute.
 * @param: command_block* block, pointer to the block
 * @param: char* token, pointer to the block
 * @return int, success 0; fail 1
 */
int push_arg(command_block* block, char* token);


#endif /* PARSER_H */