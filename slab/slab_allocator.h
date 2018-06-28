/*
slab_allocator.h
Data structures and function declarations about slab allocators
Author: Yimou Cao
Date: 12/7/2017
Course: CSCI503
Lab: #6
*/

#ifndef _SLAB_ALLOCATOR_H
#define _SLAB_ALLOCATOR_H


#define NUM_ALIGNMENT 8
#define SIZE_SLAB 1048576 // 1MB

// the 40th class's chunk is of size of 488880, the 41st class's chunk is of size of 1048576
#define MAX_NUM_CLASSES 41 

//80,96,120,152,192,240,304,384,480,600,752,944,1184,1480,
//1848,2312,2888,3608,4512,5640,7048,8808,11008,13760,17200,
//21504,26880,33600,42000,52496,65616,82016,102520,128152,
//160192,200240,250304,312880,391104,488880,1048576

struct slab_t {
	void* mem_addr;
	struct slab_t* prev;
	struct slab_t* next;
};

struct chunk_t {
	void* mem_addr;
	struct chunk_t* prev;
	struct chunk_t* next;
};

struct slab_class_t{
	unsigned long size_chunk;
	struct chunk_t* head_chunk;
	struct chunk_t* allocated_chunks;
};

struct slab_allocator
{
	unsigned long n_slabs;
	unsigned long n_classes;
	struct slab_t* pool;
	struct slab_class_t* classes;
};

/*
Initialize the slab allocator, given the pointer to it and the pool size in bytes. 
Params: alloc, the pointer to a slab allocator struct
		mem_pool_size, the size in bytes the pool would be of.
Return: no return.
The function would check if there's enough memory to create MAX_NUM_CLASSES classes.
If not, create classes as many as slabs.
The actual pool size is floored by 1MB
*/
void init_slab_allocator(struct slab_allocator* alloc, size_t mem_pool_size);

/*
Given slab allocator and num_bytes, return the pointer to an available
chunk to the user.
Params: alloc, the pointer to the slab allocator
		num_bytes, the number of bytes the user wants
Return: void*, pointer to an available chunk
*/
void* alloc_mem(struct slab_allocator* alloc, int num_bytes);

/*
free the chunk the ptr points to by moving the chunk back to the chunks list
Params: alloc, the pointer to a slab allocator
		ptr, a ptr that the user gives
Return: int, -1 mean failure; 0 means success
*/
int free_mem(struct slab_allocator* alloc, void* ptr);


/*
Popping out one slab from the pool
Params: alloc, the pointer to a slab allocator
Return: a ponter to a slab
*/
struct slab_t* popSlab(struct slab_allocator* alloc);

/*
Chop one slab into chunks with the given size of one chunk
Params: slab, the pointer to a slab
		size_chunk, size of the chunk
Return: struct chunk_t*, a pointer to the head of chunks into
		which the slab was chopped
*/
struct chunk_t* chopSlab(struct slab_t* slab, unsigned long size_chunk);

/*
Pop one chunk from the class.
Params: alloc, the pointer to the slab allocator
		the_class, the pointer to the class from which a chunk would be popped
Return: struct chunk_t*, the pointer to a chunk
*/
struct chunk_t* popChunk(struct slab_allocator* alloc, struct slab_class_t* the_class);

/*
Add the allocated chunk to the allocated list (only changing pointers)
Params: the_class, which the chunk belongs to
		chunk, the newly allocated chunk
Return: no return
*/
void addToAllocated(struct slab_class_t* the_class, struct chunk_t* chunk);


#endif /* _SLAB_ALLOCATOR_H */