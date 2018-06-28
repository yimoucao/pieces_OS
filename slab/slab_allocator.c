/*
slab_allocator.c
Function implementations about slab allocators
Author: Yimou Cao
Date: 12/7/2017
Course: CSCI503
Lab: #6
*/

#include <stdlib.h>
#include <stdio.h>

#include "slab_allocator.h"

/*
Initialize the slab allocator, given the pointer to it and the pool size in bytes. 
Params: alloc, the pointer to a slab allocator struct
		mem_pool_size, the size in bytes the pool would be of.
Return: no return.
The function would check if there's enough memory to create MAX_NUM_CLASSES classes.
If not, create classes as many as slabs.
The actual pool size is floored by 1MB
*/
void init_slab_allocator(struct slab_allocator* alloc, size_t mem_pool_size){
	if ( alloc == NULL)
	{
		fprintf(stderr, "Invalid pointer to slab_allocator\n");
		return;
	}
	// initialize the allocator with dummy values
	alloc->n_slabs = 0;
	alloc->n_classes = 0;
	alloc->pool = NULL;
	alloc->classes = NULL;

	unsigned long n_slabs = mem_pool_size / SIZE_SLAB;

    if (n_slabs < 1)
    {
        fprintf(stderr, "no pool is formed, given pool size is too small\n");
        return;
    }

	// fill the slab_allocator structure: n_slabs, n_classes
	alloc->n_slabs = n_slabs;
	if (n_slabs > MAX_NUM_CLASSES)
	{
		alloc->n_classes = MAX_NUM_CLASSES;
	}else {
		alloc->n_classes = n_slabs;
	}

	

	size_t slab_t_size = sizeof(struct slab_t);
	size_t all_classes_t_size = sizeof(struct slab_class_t) * (alloc->n_classes);
	size_t mem_size = all_classes_t_size + (SIZE_SLAB + slab_t_size) * n_slabs + (NUM_ALIGNMENT - 1);

	void* ptr = malloc(mem_size);
	if (ptr == NULL)
	{
		fprintf(stderr, "Malloc pool error\n");
		return;
	}

	void* class_t_head = ptr; // to store classes-maintaining structure

	// shifting the pointer
	ptr += all_classes_t_size;

	// shifting the ptr such that ptr is aligned
	size_t ptr_real_slabs = (size_t)ptr + slab_t_size * n_slabs;
	unsigned long remainer = ptr_real_slabs % NUM_ALIGNMENT;
	if ( remainer != 0)
	{
		ptr_real_slabs += NUM_ALIGNMENT - remainer;
	}

	// doubly linking the slabs
	int i=0;
	struct slab_t* pool_head = ptr; // pool head to store
	struct slab_t* prev = NULL;
	for (i = 0; i < n_slabs; ++i)
	{
		 struct slab_t* slab = (struct slab_t*)ptr;
		 slab->mem_addr = (void*)ptr_real_slabs;
		 slab->prev = prev;
		 slab->next = NULL;
		 if(prev!=NULL){prev->next = slab;}

		 ptr += slab_t_size;
		 ptr_real_slabs += SIZE_SLAB;
		 prev = slab;
	}

	// fill the slab_allocator structure: pool
	alloc->pool = pool_head;
	
	// populating slab classes
	struct slab_class_t* classes = (struct slab_class_t*)class_t_head;
	unsigned long CHUNK_SIZES[41] = {
			80,96,120,152,192,240,304,384,480,600,752,944,1184,1480,
			1848,2312,2888,3608,4512,5640,7048,8808,11008,13760,17200,
			21504,26880,33600,42000,52496,65616,82016,102520,128152,
			160192,200240,250304,312880,391104,488880,1048576
		};
	for (i = 0; i < alloc->n_classes; ++i)
	{
		classes[i].size_chunk = CHUNK_SIZES[i];
		struct slab_t* slab = popSlab(alloc); // get one slab
		classes[i].head_chunk = chopSlab(slab, classes[i].size_chunk); // split slab
		classes[i].allocated_chunks = NULL;
	}

	// fill the slab_allocator structure: classes
	alloc->classes = classes;
}

/*
Popping out one slab from the pool
Params: alloc, the pointer to a slab allocator
Return: a ponter to a slab
*/
struct slab_t* popSlab(struct slab_allocator* alloc){
	if ( alloc == NULL )
	{
		fprintf(stderr, "Invalid pointer to slab_allocator\n");
		return NULL;
	}

	// get one slab from the head
	struct slab_t* result = alloc->pool;
	if ( alloc->n_slabs==0 || result == NULL )
	{
		fprintf(stderr, "No available slab in the pool\n");
		return NULL;
	}
	alloc->pool = alloc->pool->next; // make the header linked to the next slab
	alloc->n_slabs -= 1;
	return result;
}


/*
Chop one slab into chunks with the given size of one chunk
Params: slab, the pointer to a slab
		size_chunk, size of the chunk
Return: struct chunk_t*, a pointer to the head of chunks into
		which the slab was chopped
*/
struct chunk_t* chopSlab(struct slab_t* slab, unsigned long size_chunk){
	if ( slab==NULL)
	{
		fprintf(stderr, "Unable to chop slab, invalid pointer\n");
		return NULL;
	}
    if ( size_chunk < 1 || size_chunk > SIZE_SLAB )
    {
        fprintf(stderr, "Unable to chop slab, invalid chunk size\n");
        return NULL;
    }

	unsigned n_chunks = SIZE_SLAB / size_chunk;
	struct chunk_t* chunks = (struct chunk_t*)malloc(n_chunks*sizeof(struct chunk_t));
	if(chunks == NULL){
		fprintf(stderr, "Unable to chop slab, malloc error\n");
		return NULL;
	}

	// spliting the slab and doubly linking the chunks
	int i=0;
	void* mem_ptr = slab->mem_addr;
	for (i = 0; i < n_chunks; ++i)
	{
		chunks[i].mem_addr = mem_ptr;
		if(i>0){
			chunks[i].prev = &chunks[i-1];
		}else {
			chunks[i].prev = NULL;
		}
		if(i<n_chunks-1){
			chunks[i].next = &chunks[i+1];
		}else {
			chunks[i].next = NULL;
		}

		mem_ptr += size_chunk;
	}
	mem_ptr = NULL;

	return chunks;
}

/*
Given slab allocator and num_bytes, return the pointer to an available
chunk to the user.
Params: alloc, the pointer to the slab allocator
		num_bytes, the number of bytes the user wants
Return: void*, pointer to an available chunk
*/
void* alloc_mem(struct slab_allocator* alloc, int num_bytes){
	if ( alloc == NULL )
	{
		fprintf(stderr, "Cannot alloc, invalid pointer\n");
		return NULL;
	}
	if ( num_bytes <= 0 )
	{
		fprintf(stderr, "Cannot alloc, invalid value of num_bytes\n");
		return NULL;
	}

	unsigned long n_classes = alloc->n_classes;
	struct slab_class_t* classes = alloc->classes;

	// find the appropriate slab class 
	int i=0;
	for (i = 0; i < n_classes; ++i)
	{
		if ( classes[i].size_chunk >= num_bytes )
		{
			break;
		}
	}

	if (i == n_classes)
	{
		fprintf(stderr, "Cannot alloc, no proper class available\n");
		return NULL;
	}

	// remove the chunk from the class
	struct chunk_t* chunk = popChunk(alloc, &classes[i]);
	if ( chunk == NULL )
	{
		return NULL;
	}

	return chunk->mem_addr; // return the void* pointer
}

/*
Pop one chunk from the class.
Params: alloc, the pointer to the slab allocator
		the_class, the pointer to the class from which a chunk would be popped
Return: struct chunk_t*, the pointer to a chunk
*/
struct chunk_t* popChunk(struct slab_allocator* alloc, struct slab_class_t* the_class){
	if ( alloc==NULL || the_class==NULL)
	{
		fprintf(stderr, "Cannot get chunk, invalid input parameter\n");
		return NULL;
	}

	if ( the_class->head_chunk == NULL )
	{	
		// the class is empty of chunks now
		struct slab_t* slab = popSlab(alloc); // get one slab from the pool
		if ( slab == NULL )
		{
			fprintf(stderr, "Cannot get chunk, empty pool\n");
			return NULL;
		}
		the_class->head_chunk = chopSlab(slab, the_class->size_chunk); // chop the slab into chunks
	}
	// get one chunk from the head of the chunks list
	struct chunk_t* result = the_class->head_chunk;

	the_class->head_chunk = the_class->head_chunk->next;
	if ( the_class->head_chunk )
	{
		the_class->head_chunk->prev = NULL;
	}

	result->prev = NULL;
	result->next = NULL;
	addToAllocated(the_class, result);

	return result;
}

/*
Add the allocated chunk to the allocated list (only changing pointers)
Params: the_class, which the chunk belongs to
		chunk, the newly allocated chunk
Return: no return
*/
void addToAllocated(struct slab_class_t* the_class, struct chunk_t* chunk){
	if ( the_class == NULL )
	{
		fprintf(stderr, "cannot mark allocated, invalid pointer\n");
		return;
	}
	if ( chunk == NULL )
	{
		// do nothing
		return;
	}

	// add to the end of the list
	if ( the_class->allocated_chunks == NULL )
	{
		the_class->allocated_chunks = chunk;
	}else {
		the_class->allocated_chunks->next = chunk;
		chunk->prev = the_class->allocated_chunks;

		the_class->allocated_chunks = chunk;
	}

	return;
}

/*
free the chunk the ptr points to by moving the chunk back to the chunks list
Params: alloc, the pointer to a slab allocator
		ptr, a ptr that the user gives
Return: int, -1 mean failure; 0 means success
*/
int free_mem(struct slab_allocator* alloc, void* ptr){
	if ( alloc == NULL )
	{
		fprintf(stderr, "cannot free, invalid allocator pointer\n");
		return -1;
	}
	if ( ptr == NULL )
	{
		fprintf(stderr, "cannot free, invalid pointer\n");
		return -1;
	}


	// find which class and which chunk this ptr belongs to
	unsigned long n_classes = alloc->n_classes;
	struct slab_class_t* classes = alloc->classes;

	int i=0;
	struct chunk_t* result_chunk = NULL;
	for (i = 0; i < n_classes; ++i)
	{
		if (classes[i].allocated_chunks){
			struct chunk_t* cursor = classes[i].allocated_chunks;
			while(cursor){
				if ( cursor->mem_addr == ptr)
				{
					result_chunk = cursor;
					break;
				}
				cursor = cursor->prev;
			}
		}


		if (result_chunk)
		{
			break;
		}
	}

	if ( result_chunk == NULL || i==n_classes )
	{
		fprintf(stderr, "cannot free, cannot find the chunk's class\n");
		return -1;
	}

    // remove from allocated list
    if (result_chunk->prev)
    {
        result_chunk->prev->next = result_chunk->next;
    }
    if (result_chunk->next)
    {
        result_chunk->next->prev = result_chunk->prev;
    }else {
        classes[i].allocated_chunks = result_chunk->prev;
    }
	result_chunk->prev = NULL;
	result_chunk->next = NULL;


	// put the chunk to the head of the chunks list
	struct chunk_t* head = classes[i].head_chunk;
	if ( head==NULL)
	{
		classes[i].head_chunk = result_chunk;
	}else {
		result_chunk->next = head;
		head->prev = result_chunk;

		classes[i].head_chunk = result_chunk;
	}

	return 0;
}