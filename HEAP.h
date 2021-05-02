

#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define HEAP_REQUIRED_SIZE    10000            // size of heap in bytes
#define NULL ((void *)0)                      // null pointer



#define StructNodeSize    ((uint32_t)sizeof(NodeTypedef_t))   //size of node equal 8 byte in 32bit machine
#define HeapAlignAddress   ((uint32_t)TotalHeapSize & ~(0x3u))  // align the heap array
#define HeapNewSize        ((HEAP_REQUIRED_SIZE)-((uint32_t)(((uint8_t *)TotalHeapSize)-((uint8_t*)HeapAlignAddress))))
#define MINI_BLOCK_SIZE    (StructNodeSize * 2)  // this make the minimum heap that we can created is 2 word size
#define TARGET_ALIGN_SIZE    0x03u


// APIs used as interface with our new heap
void    FREE_FUN(void * Block);
void *  MALLOC_FUN(uint32_t Requiredsize);
void *  CALLOC_FUN(uint32_t NumOfItems, uint32_t size);
void *  REALLOC_FUN(void *ptr, uint32_t size);










#endif
