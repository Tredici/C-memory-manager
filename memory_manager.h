
#ifndef MEMORY_MANAGER
#define MEMORY_MANAGER

int mm_init_memory_manager(void *buffer, unsigned long len);

void *mm_malloc(unsigned long size);
void mm_free(void *ptr);

#endif
