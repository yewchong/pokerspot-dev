#ifndef __gettime_h__
#define __gettime_h__

#include "includes.h"

static const char *GetTime(void);

/*
 * GetTime
 *
 * Return current time as character string.
 */
const char* GetTime()
{
  time_t ltime;
  time(&ltime); 
  return ctime(&ltime);
}

#endif