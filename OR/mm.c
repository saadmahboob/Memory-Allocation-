/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "bteam",
    /* First member's full name */
    "Eugene H. Krabs",
    /* First member's email address */
    "krabs@cs.rochester.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* 16 byte alignment */
#define ALIGNMENT 16

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT) -1) & ~(ALIGNMENT- 1))

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* global variables */
size_t TUAB; /* total allocated and used bytes */ 
size_t TFAB; /* total allcaoted but free bytes */
size_t TAB;  /* total allocated bytes */
char * PREV; /* previous meta data pointer */

/* self-defined macros and functions */
/* define meta data size */
#define CSIZE sizeof(char *)
#define SSIZE sizeof(size_t)
#define ISIZE sizeof(int)
#define RMSIZE (CSIZE+CSIZE+SSIZE+ISIZE)
#define MSIZE ALIGN(RMSIZE)

/* getters and setters for the meta data*/
char * metaNext(char * metaData)
{
    return *(char* *)metaData;
}
void metaSetNext(char * metaData, char * value)
{
     *(char* *)metaData = value;
}

char * metaPrev(char * metaData)
{
    char * addr = (char *)((int)metaData + CSIZE);
    return *(char* *)addr;
}
void metaSetPrev(char * metaData, char * value)
{
     char * addr = (char *)((int)metaData + CSIZE);
     *(char* *)addr = value;
}

size_t metaSize(char * metaData)
{
    char * addr = (char *)((int)metaData + CSIZE+CSIZE);
    return *(size_t *)addr;
}
void metaSetSize(char * metaData, size_t value)
{
     char * addr = (char *)((int)metaData + CSIZE+CSIZE);
     *(size_t *)addr = value;
}

int metaStatus(char * metaData)
{
    char * addr = (char *)((int)metaData + CSIZE+CSIZE+SSIZE);
    return *(int *)addr;
}
void metaSetStatus(char * metaData, int value)
{
     char * addr = (char *)((int)metaData + CSIZE+CSIZE+SSIZE);
     *(int *)addr = value;
}
/* return the start address of a memory block */
char *metaBlockStart(char * metaData)
{
     char * addr = (char *)((int)metaData +MSIZE);
     return addr;
}
/* return the meta data's address if given a block's address */
char *blockMetaStart(char * block)
{
     char * addr = (char *)((int)block -MSIZE);
     if( addr >= (char *)mem_heap_lo())
          return addr;
     else
          return (char *)(-1);
}

/* helper functions lv2 */
/* size is already aligned */
/* return style 1. pointer points to a metadata 2. -1*/
char * find_free_block(size_t size)
{
     char * current = mem_heap_lo();
     if(mem_heapsize() == 0)
          return (char *)(-1);
     else
     {
          while(current < (char *)mem_heap_hi())
          {
               if((metaStatus(current) == 0) && (metaSize(current) >= size))
                    return current; 
               else
               {
                    if(strcmp(current,"NULL")!=0)     
                         current = metaNext(current);
                    else
                         return (char *)(-1);
               }
          }
     }
}
/* split a free block(size_t, char * metadata) */
char * split(size_t size, char * metaData)
{
     size_t oldsize = metaSize(metaData);
     metaSetSize(metaData,size);
     metaSetStatus(metaData,1);

     char * new = (char *)((int)metaBlockStart(metaData)+size);
     metaSetNext(new, metaNext(metaData));
     metaSetPrev(new,metaData);

     metaSetNext(metaData,new);
     metaSetSize(new,oldsize-size-MSIZE);
     metaSetStatus(new,0);

     return metaBlockStart(metaData);
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);

    if (p == (void *)-1) {
	    return NULL;
    } else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) { }

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL) {
      return NULL;
    }

    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize) {
      copySize = size;
    }

    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);

    return newptr;
}
