/** 
 * @file main.c
 * @author Yimou Cao
 * @date 09/08/2017
 * Course: CSCI503
 * Lab: 1
 *
 * @brief this file contains some test cases.
 */

#include <stdio.h>
#include "balloc.h"

/* print out pool's information, including pool pointer address, head pointer address, number of all blocks
 * alignment bytes and available blocks */
void printPoolInfo(block_pool* pool)
{
	printf("pool pointer: %lu;head pointer: %lu; all: %d; align: %d; avail: %d\n", 
		(size_t)pool, (size_t)(pool->pool_head), pool->total_num_blocks, pool->alignment_bytes, pool->num_avail_blocks);
}


/* Some tests triggering specific error detections. The while loop tests block allocation. The first for-loop tests 
 * freeing blocks into pool. The second for-loop tests if a block is already freed into pool. */
void test()
{
	printf("---test starts!\n");

	block_pool* pool = create_block_pool(4, 4096, 17);
	printPoolInfo(pool);
	
	int n=5;
	void* blocks[5];
	while(n--)
	{
		printf("[while: %d]\n", n);
		void* block1 = alloc_align_block(pool);
		blocks[n] = block1;
		printf("chunk address: %lu\n", (size_t)block1);
		printPoolInfo(pool);	
		printf("\n");
	}

	// free_align_block(pool, block1);
	// printPoolInfo(pool);
	int i;
	for ( i = 0; i < 4; ++i)
	{
		printf("[free: %d]\n", i);
		free_align_block(pool, blocks[i]);
		printPoolInfo(pool);
		printf("\n");
	}

	for ( i = 0; i < 5; ++i)
	{
		printf("[free: %d]\n", i);
		free_align_block(pool, blocks[i]);
		printPoolInfo(pool);
		printf("\n");
	}

	printf("---test end---\n");
}

/* This is a sample */
void sample1()
{	
	//create a pool
	block_pool* pool = create_block_pool(100, 2048, 17);
	//fetch one block
	void* block1 = alloc_align_block(pool);
	//free one block (put it back into pool)
	free_align_block(pool, block1);
}

int main(int argc, char const *argv[])
{
	test();
	sample1();
	return 0;
}