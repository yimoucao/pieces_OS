/** 
 * @file parser.c
 * @author Yimou Cao
 * @date 09/22/2017
 * Course: CSCI503
 * Lab: 2
 *
 * @brief parse inputs from user, including readline, parseline into commands
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

/* Read a line from user
 * @param: no param
 * @return char*, pointer to one line string
 * Through while loop it continuously get characters one by one until EOF or line feed is
 * received.
 */
char* readline()
{
	int buffer_size = NUM_CHARS;
	int pos = 0;
	char *buffer = malloc(sizeof(char) * buffer_size);
	int c; //store char obrained from input
	bool backspace_suggested = false; // make sure suggestion only appear once

	if (!buffer) 
	{
		perror("[Shell ERROR] reading command error (allocation failed)");
		exit(EXIT_FAILURE);
	}

	while (1) {
		// Read a character
		c = getchar();

		// If we hit EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') {
		  	buffer[pos] = '\0';
		  	return buffer;
		}
		else if ( c == '\b')
		{
			if ( !backspace_suggested )
			{
				fprintf(stderr, "[Shell Suggest] In some cases use ctrl+backspace to do interactive back deleting\n");
				backspace_suggested = true;
			}
		}
		else 
		{
		  	buffer[pos] = c;
			pos++;
		}

		// If we have exceeded the buffer, reallocate.
		if (pos >= buffer_size) {
			buffer_size += NUM_CHARS;
			buffer = realloc(buffer, buffer_size);
			if (!buffer) {
				perror("[Shell ERROR] reading command error (allocation failed)");
				exit(EXIT_FAILURE);
			}
		}
	}

	// scanf("%s",buffer);
	// return buffer;
}


/* Tokenize one line into an array of tokens.
 * @param: char* line, a line of characters from user
 * @param: int* num, pointer to an int, write into it how many tokens in the line after 
 * parsing done
 * @return char**, pointer of an array of tokens
 * Through while loop it keeps separating words by specified delimeters.
 */
char** tokenize(char* line, int* num)
{
	int token_buf_size = TOKEN_BUF_SIZE;
	int pos = 0;
	char **tokens = malloc(token_buf_size * sizeof(char*));
	char *token;

	if (!tokens) {
		perror("[Shell ERROR] reading command error (token buffer allocation failed)");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_DELIM); //orginal string's address
	while (token != NULL) 
	{
		tokens[pos] = token;
		pos++;

		// if buffer is full, realloc
		if (pos >= token_buf_size) 
		{
			token_buf_size += TOKEN_BUF_SIZE;
			tokens = realloc(tokens, token_buf_size * sizeof(char*));
			if (!tokens) {
				perror("[Shell ERROR] reading command error (token buffer reallocation failed)");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_DELIM);
	}
	tokens[pos] = NULL; //set an end that exec() needs
	*(num) = pos; // write into num how many tokens are obtained
	return tokens;
}


/* Initialize the command block struct.
 * @param: command_block* block, pointer to the block
 * @return void
 */
void command_block_init(command_block* block)
{
	block->args_buf_size = ARGS_BUF_SIZE;
	block->args = (char**) malloc( (block->args_buf_size) * sizeof(char*));
	if ( !(block->args) )
	{
		perror("[Shell ERROR] command block struct init error");
		exit(EXIT_FAILURE);
	}
	block->num_args = 0;
	block->flag = 0;
	block->in_file = NULL;
	block->out_file = NULL;
	block->next = NULL;

	return;
}


/* Push one token into the command block's args attribute.
 * @param: command_block* block, pointer to the block
 * @param: char* token, pointer to the block
 * @return int, success 0; fail 1
 */
int push_arg(command_block* block, char* token)
{
	(block->args)[block->num_args] = token;
	(block->num_args)++; //increment number of arguments
	(block->args)[block->num_args] = NULL;

	// if args space is full, realloc
	if ((block->num_args) + 1 >= (block->args_buf_size)) //always give a space for NULL
	{
		block->args_buf_size += ARGS_BUF_SIZE;
		block->args = realloc(block->args, (block->args_buf_size) * sizeof(char*));
		if ( !(block->args) )
		{
			perror("[Shell ERROR] command args realloc error");
			exit(EXIT_FAILURE);
		}
	}

	return 0; // success 0; fail 1

}


/* Group tokens into different commands. These commands are represented by command
 * blocks. Piped commands are linked together through a linked list
 * @param: int num, number of tokens in one line.
 * @param: char** tokens, an array of tokens
 * @param: bool* bg_flag, pointer to a bool, to be written with backround flag
 * @return command_block*, the header command block of the linked list
 * The brief steps are as following
 * 1. check if the input parameters are valid
 * 2. check if the last token is amper. If so, set background flag
 * 3. through the while loop, compare each token with special symbols and perform
 * corresponding actions
 */
command_block* commandize(int num, char** tokens, bool* bg_flag)
{
	if (num == 0 || tokens == NULL || bg_flag == NULL)
	{
		return NULL;
	}
	if(strcmp(tokens[num-1],"&") ==0)
    {
        tokens[num-1] = NULL;
        num--;
        *(bg_flag) = true;
        #ifdef DEBUG
        printf("compaing and has amper\n");
		#endif
    }

    command_block* c_block = (command_block*) malloc(sizeof(command_block));
    if ( !c_block )
	{
		perror("[Shell ERROR] a command block malloc error");
		exit(EXIT_FAILURE);
	}
    command_block_init(c_block);
    command_block* cur = c_block;
    command_block* header = cur;

    int i = 0;
    while(i < num)
    {
    	#ifdef DEBUG
    	printf("parsing token: %s\n", tokens[i]);
		#endif
    	if(strcmp(tokens[i],"<<")==0 || strcmp(tokens[i],"<")==0)
        {
            c_block->flag |= IN_REDIRECTED;
            c_block->in_file = tokens[i+1];
            tokens[i] = NULL;
            i+=2;
        }
        else if(strcmp(tokens[i],">")==0)
        {
            c_block->flag |= OUT_REDIRECTED;
            c_block->out_file = tokens[i+1];
            tokens[i] = NULL;
            i+=2;
        }
        else if(strcmp(tokens[i],">>")==0)
        {
            c_block->flag |= OUT_REDIRECTED_APPEND;
            c_block->out_file = tokens[i+1];
            tokens[i] = NULL;
            i+=2;
        }
        else if(strcmp(tokens[i],"|")==0)
        {
            c_block->flag |= PIPIED;
            tokens[i] = NULL;

            //new command block
            c_block = (command_block*) malloc(sizeof(command_block));
            if ( !c_block )
			{
				perror("[Shell ERROR] a command block malloc error");
				exit(EXIT_FAILURE);
			}
		    command_block_init(c_block);
		    // link together blocks
            cur->next = c_block;
            cur = c_block;
            i++;
        }
        else
        {
        	push_arg(c_block, tokens[i]); //push token into this command's args list
        	#ifdef DEBUG
        	printf("pushing arg: %s\n", tokens[i]);
			#endif
        	i++;
        }
    }
    return header;
}


/* Parse a line.
 * @param: char* line, a line of characters
 * @param: bool* bg_flag, pointer to a bool, to be written with backround flag
 * @return command_block*, the header command block of the linked list
 * The brief steps are as following
 * 1. tokenize
 * 2. commandize
 * 3. if the application is compiled with debug option, print out commands and args
 */
command_block* parseline(char* line, bool* bg_flag)
{
	int token_nums = 0;
	char** tokens = tokenize(line, &token_nums);

	#ifdef DEBUG
	printf("token numbers: %d\n", token_nums);
	#endif


	command_block* one = commandize(token_nums, tokens, bg_flag);

	#ifdef DEBUG
	printf("packing finished\n");
	command_block* cmd = one;
	
	while(cmd)
	{
		char** myargs = cmd->args;
		while(myargs[0]){
			printf("arg: %s\t", myargs[0]);
			myargs++;
		}
		printf("\n");
		cmd = cmd->next;

	}
	printf("printing commands finished\n");
	#endif /* end define DEBUG */

	return one;
}