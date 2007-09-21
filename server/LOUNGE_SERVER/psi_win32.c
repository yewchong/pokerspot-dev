#include "psi_win32.h" 
#include "includes.h"
#include <time.h>


void gettimeofday(struct timeval* ptv, void* notused)
{
  time_t t;
  time(&t);
  ptv->tv_sec = t;
  ptv->tv_usec = 0;
}

/*int bzero(void* data, size_t numbytes)
{
  return (int)memset(data, 0, numbytes);
}*/
