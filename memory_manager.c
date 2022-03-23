#include "memory_manager.h"

/**
 * Cannot rely on existent libraries for OS code
 */
#define NULL (void*)0

struct memory_segment
{
    struct memory_segment *prev, *next;
    unsigned long len;
};

struct managed_memory
{
    struct memory_segment *buffer;
    unsigned long len;
    int ready;
};

struct managed_memory memory;

int mm_init_memory_manager(void *buffer, unsigned long len)
{
    if (!len || !buffer || 10*sizeof(struct memory_segment) > len)
    {
        return -1;
    }
   // zero all memory
    for (char *p = (char *)buffer, *limit = (char *)buffer+len;
        p < limit; ++p)
    {
        *p++ = 0;
    }
    // prepare memory manager data
    memory.buffer = (struct memory_segment *)buffer;
    memory.len = len;
    memory.ready = 1;
    // clear first element
    *memory.buffer = (struct memory_segment){};
    return 0;
}

/**
 * Iterate over all available segments
 * looking for available space
 */
void *mm_malloc(unsigned long size)
{
    if (!size && !memory.ready) // cannot allocate empty segments
    {
        return NULL;
    }
    // get pointer to first segment
    struct memory_segment *iter = memory.buffer;
    // Iterate over all segments
    while (iter)
    {
        if (iter->next)
        {
            // this segment is partially occupied?
            if (iter->len)
            {
                // check if there is enough space before next
                // pay attention that 2 segments headers must
                // be considered (current and new)
                if ((char*)iter->next - (char*)iter >= iter->len + 2*sizeof(*iter) + size)
                {
                    // get position of new segment header
                    struct memory_segment *const news = (struct memory_segment *)
                        ((char *)(iter+1) + iter->len);
                    // prepare memory
                    *news = (struct memory_segment){};
                    // new segment size
                    news->len = size;
                    // set previous and next
                    news->prev = iter; news->next = iter->next;
                    news->next->prev = news; news->prev->next = news;
                    // return pointer to new place
                    return news+1;
                }
            }
            else // found empty segment!
            {
                // in this case no new header must be inserted
                if ((char*)iter->next - (char*)iter >= sizeof(*iter) + size)
                {
                    // we can use the old segment
                    iter->len = size;
                    return iter+1;
                }
            }
        }
        else // reached last segment
        {
            // upper bound of the allocable memory
            char *const upper_boud = (char*)memory.buffer + memory.len;
            if (iter->len) // must allocate a new item
            {
                // candidate new segment header
                struct memory_segment *const news = (struct memory_segment*)
                    ((char*)(iter+1) + iter->len);
                // enough space?
                if ((char*)(news+1) + size <= upper_boud)
                {
                    *news = (struct memory_segment){};
                    news->len = size;
                    news->prev = iter;
                    iter->next = news;
                    return news+1;
                }
            }
            else // try to recycle if it was empty
            {
                if ((char*)(iter+1) + size <= upper_boud)
                {
                    iter->len = size;
                    return iter+1;
                }
            }
            return NULL;
        }
        iter = iter->next;
    } 
    return NULL;
}

/**
 * Free memory associated with the given pointer.
 * Point shoud point to a byte immediately following
 * a struct memory_segment item.
 */
void mm_free(void *ptr)
{
    if (!ptr)
        return;
    // get reference to relative struct memory_segment item
    struct memory_segment *const item = (struct memory_segment *)ptr - 1;
    // Case 1: not the first of the list
    if (item->prev)
    {
        if (item->len) // prevent double free
        {
            item->len = 0;
            // change previous' next
            item->prev->next = item->next;
            if (item->next) // If next exists 
            {
                // update next's previous
                item->next->prev = item->prev;
            }
        }
    }
    else // working on the first segment
    {
        if (item->len) // prevent double free
        {
            item->len = 0;
        }
    }
}
