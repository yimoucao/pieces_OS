
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "slab_allocator.h"

int main(int argc, char const *argv[])
{
	// stack-ly create slab_allocator
	// fill the slab_allocator by calling init_
    struct slab_allocator* alloc = (struct slab_allocator*)malloc(sizeof(struct slab_allocator));
    if ( alloc==NULL)
    {
        fprintf(stderr, "cannot create alloctor\n");
        return 0;
    }

    init_slab_allocator(alloc, 41*pow(2,20));
    
    int i=0;
    int n=20;
    void* ptrs[n];
    for (i = 0; i < n; ++i)
    {
        ptrs[i] = alloc_mem(alloc, 200240);
        if (ptrs[i] == NULL )
        {
            printf("get NULL chunk\n");
        }else {
            printf("%lu, %%8? %lu\n", (size_t)ptrs[i], (size_t)ptrs[i]%8);
        }
        // printf("free? %d\n", free_mem(alloc, ptrs[i]));
    }

    for (i = 0; i < n; ++i)
    {
        printf("free? %d\n", free_mem(alloc, ptrs[i]));
    }

    for (i = 0; i < n; ++i)
    {
        ptrs[i] = alloc_mem(alloc, 200240);
        if (ptrs[i] == NULL )
        {
            printf("get NULL chunk\n");
        }else {
            printf("%lu, %%8? %lu\n", (size_t)ptrs[i], (size_t)ptrs[i]%8);
        }
        // printf("free? %d\n", free_mem(alloc, ptrs[i]));
    }

	return 0;
}