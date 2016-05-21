#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

typedef struct 
{
    byte* memory;
    int memoryOffset, size;
} MemoryPool;

void memoryPoolInit(MemoryPool* m, int size);
void* memoryPoolAllocate(MemoryPool* m, int size);
void memoryPoolClear(MemoryPool* m);

#endif // MEMORY_POOL_H