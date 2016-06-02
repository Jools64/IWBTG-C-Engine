
// A generic dynamically growing array.

/*
    TODO:
    - Keep track of the last empty slot in each bucket for quick allocation.
    - Set and Get element by index.
*/

typedef struct BucketArray BucketArray;
typedef struct BucketArray
{
    unsigned char* data; // Memory for this bucket
    unsigned char* elementUsage; // Elements in this bucket that are in use
    
    BucketArray* nextBucket;
    
    int elementSize, // Size of the elements in this bucketArray
        elementsPerBucket, // Number of elements within a single bucket
        elementCount; // The number of elements in this bucket
    void* firstAddress,  * lastAddress;
} BucketArray;

typedef struct BucketArrayIterator
{
    BucketArray* bucket; 
    int position;
    BucketArray* bucketArray;
} BucketArrayIterator;

void bucketArrayInit(BucketArray* b, int elementSize, int elementsPerBucket)
{
    b->elementSize = elementSize;
    b->elementsPerBucket = elementsPerBucket;
    b->data = malloc(elementSize * elementsPerBucket);
    b->elementUsage = calloc(sizeof(unsigned char), elementsPerBucket);
    b->firstAddress = (void*)(&b->data[0]);
    b->lastAddress = (void*)&b->data[elementsPerBucket * elementSize];
    b->elementCount = 0;
    b->nextBucket = 0;
}

void bucketArrayDestroy(BucketArray* b)
{
    if(b->nextBucket)
    {
        bucketArrayDestroy(b->nextBucket);
        free(b->nextBucket);
    }
    free(b->data);
    free(b->elementUsage);
}

BucketArray* bucketArrayGetLastBucket(BucketArray* b)
{
    while(b->nextBucket != 0)
        b = b->nextBucket;
    return b;
}

void bucketArrayAddBucket(BucketArray* b)
{
    b = bucketArrayGetLastBucket(b);
    b->nextBucket = calloc(sizeof(BucketArray), 1);
    bucketArrayInit(b->nextBucket, b->elementSize, b->elementsPerBucket);
}

unsigned char bucketArrayIsFull(BucketArray* b)
{
    if(b->elementCount >= b->elementsPerBucket)
        return true;
    return false;
}

BucketArray* bucketArrayGetFirstWithSpace(BucketArray* b)
{
    while(bucketArrayIsFull(b))
    {
        if(b->nextBucket == 0)
            bucketArrayAddBucket(b);
        b = b->nextBucket; 
    }
    return b;
}

void* bucketArrayAllocate(BucketArray* b)
{
    b = bucketArrayGetFirstWithSpace(b);
    for(int i = 0; i < b->elementsPerBucket; ++i)
        if(!b->elementUsage[i])
        {
            b->elementUsage[i] = 1;
            b->elementCount++;
            void* data = (void*)(&b->data[i * b->elementSize]);
            return data;
        }
    // TODO: Invalid code path
    return 0;
}

unsigned char bucketArrayBucketContainsAddress(BucketArray* b, void* address)
{
    return address >= b->firstAddress && address <= b->lastAddress;
}

BucketArray* bucketArrayFindBucketWithAddress(BucketArray* b, void* address)
{
    while(b != 0)
    {
        if(bucketArrayBucketContainsAddress(b, address))
            return b;
        b = b->nextBucket;
    }
    
    return 0;
}

unsigned char bucketArrayDeallocate(BucketArray* b, void* address)
{
    b = bucketArrayFindBucketWithAddress(b, address);
    if(b)
    {
        int offset = ((uintptr_t)address - (uintptr_t)b->firstAddress);
        int index = offset / b->elementSize;
        b->elementUsage[index] = false;
        b->elementCount--;
        return 1;
    }
    
    return 0;
}

unsigned char bucketArrayRemove(BucketArray* b, void* element)
{
    return bucketArrayDeallocate(b, element);
}

void* bucketArrayAdd(BucketArray* b, void* element)
{
    void* e = bucketArrayAllocate(b);
    memcpy(e, element, b->elementSize);
    return e;
}

void bucketArrayIteratorInit(BucketArrayIterator* it, BucketArray* b)
{
    it->position = 0;
    it->bucket = b;
}

void* bucketArrayIteratorNext(BucketArrayIterator* it)
{
    if(it->bucket == 0)
        return 0;
    
    for(; it->position < it->bucket->elementsPerBucket; ++it->position)
    {
        
        if(it->bucket->elementUsage[it->position])
        {
            void* value = (void*)(&it->bucket->data[it->position * it->bucket->elementSize]);
            it->position++;
            if(it->position == it->bucket->elementsPerBucket)
            {
                it->position = 0;
                it->bucket = it->bucket->nextBucket;
            }
            return value;
        }
        printf("Nothing here %d \n", it->position);
    }
    
    it->bucket = it->bucket->nextBucket;
    it->position = 0;
    return bucketArrayIteratorNext(it);
}