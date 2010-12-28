
#include <windows.h>

/*
void * __attribute__((__stdcall__)) VirtualAlloc(void *, unsigned long, unsigned long, unsigned long);
*/

#include "mem-segment.h"

unsigned char *
mem_segment_alloc (int sz)
{
  void * p = ::VirtualAlloc(0, sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  return (unsigned char *) p;
}
