/** 
 * @file balloc.h
 * @author Yimou Cao
 * @date 09/08/2017
 * Course: CSCI503
 * Lab: 1
 *
 * @brief File containing interfaces exposed to programmers.
 */

typedef char bool;
#define true 1
#define false 0

/* structure of a block and define the type of blocks */
struct block
{
    void* chunk;
    struct block* prev;
    struct block* next;
};
typedef struct block block;

/* structure of a block pool and define the type of block pools */
struct block_pool
{
    struct block* pool_head;
    int total_num_blocks;
    int alignment_bytes;
    int num_avail_blocks;
};
typedef struct block_pool block_pool;


/* Declaration of creating a pool of blocks.
 * @param int num_blocks: the number of blocks
 * @param int block_size: size in bytes for each block
 * @param int alignment: value a block should be aligned to
 * @return block_pool pointer: return NULL if malloc failed.
 * users either get a pool with their requirements met or nothing.
*/
block_pool* create_block_pool(int num_blocks, int block_size, int alignment);


/* Declaration of allocating a block from the pool to users.
 * @param struct block_pool* pool: pointer to a pool from which 
 * users want blocks from.
 * @return void pointer: return a pointer pointed to the block. 
 * return NULL if malloc failed.
 * @brief allocate a block from the pool to users. Detach the node
 * from its double linked list, adjust pool head and number of 
 * available blocks.
 * If pool pointer or pool head pointer invalid, return NULL
 * If no available blocks, return NULL and error messages would print out
*/
void* alloc_align_block(struct block_pool* pool);


/* Delcaration of freeing a block from users.
 * @param struct block_pool* pool: pointer to a pool from which 
 * users want blocks from.
 * @param void* blk: a pointer to a block users give
 * @return void: no return value; return directly if any operation
 * failed.
 * @brief fisrly check parameters are valid or not; and then check
 * if the pool is full; and then check if the block is aligned or not, 
 * it indicates the address is changed if not; and then check if the 
 * block if already freed into the pool. Finally put the blk into 
 * the pool with * header, links and number of available blocks adjusted.
 * If any error occurred in the process, error messages would print out
*/
void free_align_block(struct block_pool* pool, void* blk);