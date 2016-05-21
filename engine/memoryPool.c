void memoryPoolInit(MemoryPool* m, int size)
{
    m->memory = (byte*)malloc(size);
    m->memoryOffset = 0;
    m->size = size;
}

void* memoryPoolAllocate(MemoryPool* m, int size)
{
    if(m->memoryOffset + size > m->size)
    {
        printf("Warning: Allocator is out of memory!\n");
        return null;
    }
    int offset = m->memoryOffset;
    m->memoryOffset += size;
    return (void*)(&m->memory[offset]);
}

void memoryPoolClear(MemoryPool* m)
{
    m->memoryOffset = 0;
}