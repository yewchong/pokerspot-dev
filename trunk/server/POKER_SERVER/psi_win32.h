#ifndef __psi_win32_h__
#define __psi_win32_h__


#ifdef _WIN32
#include <windows.h>

extern void gettimeofday(struct timeval*, void*);
extern int bzero(void *, size_t);
extern void InitWinSock(void);

typedef unsigned char  u_byte_t;
typedef unsigned short u_int16_t;
typedef unsigned long  u_int32_t;
typedef DWORD64        my_ulonglong;

  #define ssize_t size_t
  /* map bzero to memset */
  #define bzero(x, y)	memset(x, 0x0, y)
  /* map read syscall to recv */
  //#define read(x, y, z) recv((x), (y), (z), 0)
  /* map write syscall to send */
  //#define write(x, y, z) send((x), (y), (z), 0)
  /* map fcntl to ioctlsocket */
  #define fcntl(s, f) ioctlsocket((s), FIONBIO, &nblock)
  /* map close to closesocket */
  #define close(s) closesocket((s))
  /* used by ioctlsocket */
  //extern u_long nblock;
#endif



#endif