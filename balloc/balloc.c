/** 
 * @file balloc.c
 * @author Yimou Cao
 * @date 09/08/2017
 * Course: CSCI503
 * Lab: 1
 *
 * @brief File containing implementations of those interfaces
 */

#include <stdlib.h>
#include <stdio.h>
#include "balloc.h"

/* Error messages */
#define ERR201 "Allocate block failed: pool pointer or pool_head is not valid; Or no available blocks from the pool\n"
#define ERR301 "Free block failed: pool pointer or blk pointer is not valid.\n"
#define ERR302 "Free block failed: pool is full.\n"
#define ERR303 "Free block failed: blk pointer is not aligned any more, it may have been changed.\n"
#define ERR304 "No need to free: the blk is already in the pool.\n"
#define ERR305 "Free block failed: node allocation failed\n"

/* Internal function of malloc aligned blocks
 * @param size_t size: the number of bytes of a block
 * @param int alignment: value a block should be aligned to
 * @return void pointer: return NULL if malloc failed.
 * @brief allocate a chunk of memory whose size is the requested + 
 * padding size. Increase the pointer of this chunk by the padding
 * size and decrease it to a address that is the multiple of the
 * alignment.
*/
void* aligned_malloc(size_t size, int alignment)
{
	void* result_ptr;
	void* ptr = malloc(size + alignment -1 + sizeof(void*));
	if ( ptr != NULL)
	{
		size_t addr = (size_t)ptr + alignment - 1 + sizeof(size_t);
		result_ptr = (void*)(addr - (addr % alignment));

		/* store the address of the whole chunk in the back of the 
		   result pointer. When free action is wanted, alinged_free()
		   will find the addreee and free the whole chunk.
		*/
		*((void**)((size_t)result_ptr - sizeof(void*))) = ptr;
		return result_ptr;
	}
	return NULL;
}

/* Implementation of free memory. This function is not used in this 
 * lab, only for the futural use.
 */
void aligned_free(void *p) 
{
    void *ptr = *((void**)((size_t)p - sizeof(void*)));
    free(ptr);
    return;
}


/* Implementation of creating a pool of blocks.
 * @param int num_blocks: the number of blocks
 * @param int block_size: size in bytes for each block
 * @param int alignment: value a block should be aligned to
 * @return block_pool pointer: return NULL if malloc failed.
 * @brief allocate a chunk of memory whose size is the requested + 
 * padding size. Increase the pointer of this chunk by the padding
 * size and decrease it to a address that is the multiple of the
 * alignment.
*/
block_pool* create_block_pool(int num_blocks, int block_size, int alignment)
{
	if (num_blocks <= 0 || block_size <= 0 || alignment <= 0 )
	{
		return NULL;
	}

	/* start with a null header, which also can be used as a cursor to form 
	the double linked list */
	block* cursor = (block*)malloc(sizeof(block));
	if ( !cursor )
	{
		return NULL;
	}
	cursor->chunk = NULL;
	cursor->prev = NULL;
	cursor->next = NULL;

	/* allocate some space to maintain the double linked list and initialize 
	with some values */
	block_pool* pool = (block_pool*)malloc(sizeof(block_pool));
	if ( !pool )
	{
		return NULL;
	}
	pool->alignment_bytes = alignment;
	pool->total_num_blocks = num_blocks;	
	pool->num_avail_blocks = num_blocks;

	int counter = num_blocks;
	while(counter--)
	{
		void* chunk = aligned_malloc(block_size, alignment);
		block* node = (block*)malloc(sizeof(block));
		if ( !chunk || !node)
		{
			/* if allocation failed, return NULL pool. Therefore, users 
			either get a pool with their requirements met or nothing */
			return NULL;
		}

		node->chunk = chunk;
		node->prev = cursor;
		cursor->next = node;
		cursor = node;
	}

	/* go the beginning the list and assign head pointer */
	while(cursor->prev)
	{
		cursor = cursor->prev;
	}
	pool->pool_head = cursor->next;

	return pool;
}


/* Implementation of allocating a block from the pool to users.
 * @param struct block_pool* pool: pointer to a pool from which 
 * users want blocks from.
 * @return void pointer: return a pointer pointed to the block. 
 * return NULL if malloc failed.
 * @brief allocate a block from the pool to users. Detach the node
 * from its double linked list, adjust pool head and number of 
 * available blocks.
*/
void* alloc_align_block(struct block_pool* pool)
{
	if (pool && pool->pool_head && pool->num_avail_blocks > 0)
	{
		void* result = pool->pool_head->chunk;
		pool->num_avail_blocks -= 1;
		pool->pool_head = pool->pool_head->next;
		return result;
	}
	else
	{
		printf(ERR201);
		return NULL;
	}

	return NULL;
}


/* Implementation of freeing a block from users.
 * @param struct block_pool* pool: pointer to a pool from which 
 * users want blocks from.
 * @param void* blk: a pointer to a block users give
 * @return void: no return value; return directly if any operation
 * failed.
 * @brief fisrly check parameters are valid or not. and then check
 * the blk is aligned or not. Finally put the blk into the pool with
 * header, links and number of available blocks adjusted.
*/
void free_align_block(struct block_pool* pool, void* blk)
{
	/* if pool pointer or blk pointer is invalid, print error message
	   and return
	*/
	if ( !pool || !blk)
	{
		printf(ERR301);
		return;
	}

	/* if the pool is full, print error message and return*/
	if (pool->num_avail_blocks >= pool->total_num_blocks)
	{
		printf(ERR302);
		return;
	}

	/* if blk pointer is not aligned by the aligning number of the pool
	   it indicates that the blk pointer may have been changed. At this
	   time, print error message and return
	*/
	if ( (size_t)blk % (pool->alignment_bytes))
	{
		printf(ERR303);
		return;
	}

	/* check if blk is already in the pool*/
	block* cursor = pool->pool_head;
	while(cursor)
	{
		if (cursor->chunk == blk)
		{
			printf(ERR304);
			return;
		}
		cursor = cursor->next;
	}

	block* node = (block*)malloc(sizeof(block));
	if ( !node )
	{
		printf(ERR305);
		return;
	}
	//TODO: what if blk size, we don't know...

	
	/* put the blk into the pool, set up the links and params of the pool */
	node->chunk = blk;
	node->next = pool->pool_head;
	node->prev = NULL;
	pool->num_avail_blocks += 1;
	pool->pool_head = node;
	
	return;
}