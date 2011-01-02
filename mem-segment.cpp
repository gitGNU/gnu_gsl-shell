
#include "mem-segment.h"

#ifdef WIN32
#include <windows.h>

/*
void * __attribute__((__stdcall__)) VirtualAlloc(void *, unsigned long, unsigned long, unsigned long);
*/

unsigned char *
mem_segment_alloc (int sz)
{
  void * p = ::VirtualAlloc(0, sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  return (unsigned char *) p;
}
#else

#define __USE_MISC
#include <sys/mman.h>

unsigned char *
mem_segment_alloc (int sz)
{
  void * p = mmap(0, sz, 
		  PROT_EXEC | PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  return (unsigned char *) p;
}

#endif
