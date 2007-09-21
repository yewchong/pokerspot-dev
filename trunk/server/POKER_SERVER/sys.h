#ifndef __psi_sys_h__
#define __psi_sys_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : sys.h
  Created  : 01/01/2001

  OVERVIEW : System dependent misc functions.

 ****************************************************************************/


#include "includes.h"
#include "compat.h"

extern bool Sys_InitErrorLog(const char* fname);
extern void Sys_LogError(const char* msg);
extern void Sys_PrintError(const char* msg);

extern bool Sys_InitChipLog(const char* fname);
extern void Sys_LogChipBuyin(const char* player,
                             double totalChips,
                             double buyinChips);
extern void Sys_LogChipSave(const char* player,
                            double totalChips,
                            double saveChips);

extern const char* Sys_GetDate();

extern bool Sys_InitSocket();
extern void Sys_ShutdownSocket();

extern void Sys_EnableSSL();
extern void Sys_DisableSSL();
extern bool Sys_IsSSLEnabled();
// Socket routines
extern int Sys_setnonblockio(int sockfd);
extern int Sys_setnonagle(int sockfd);
extern int Sys_accept(int listenfd, struct sockaddr* cliaddr, int* addrlen);
extern int Sys_peek(int sockfd, char* buffer, int nbytes);
extern int Sys_pending(int sockfd);
extern int Sys_recv(int sockfd, char* buffer, int nbytes);
extern int Sys_send(int sockfd, const char* buffer, int nbytes);
extern int Sys_peek(int sockfd, char* buffer, int nbytes);
extern int Sys_recv(int sockfd, char* buffer, int nbytes);
extern int Sys_send(int sockfd, const char* buffer, int nbytes);
extern void Sys_close(int sockfd);
extern void Sys_alarm(int);

extern int Sys_accept_SSL(int);

// Thread creation/ending 
extern int Sys_CreateThread(ThreadFunc, void* args);
extern void Sys_EndThread();
extern bool Sys_IsMainThread(); 
extern bool Sys_IsGameLogicThread();

// Event/Condition
extern HANDLE Sys_CreateEvent();
extern int Sys_SetEvent(HANDLE);
extern int Sys_ResetEvent(HANDLE);
extern int Sys_PulseEvent(HANDLE);
extern int Sys_WaitEvent(HANDLE, DWORD msecs);
extern void Sys_DestroyEvent(HANDLE);

// Mutex
extern HANDLE Sys_CreateMutex();
extern int Sys_LockMutex(HANDLE, DWORD msecs);
extern int Sys_ReleaseMutex(HANDLE);
extern int Sys_DestroyMutex(HANDLE);

// Misc
extern DWORD Sys_GetTicks();
extern void Sys_Sleep(DWORD msecs);
extern void Sys_OutputDebugString(const char*);
extern int  Sys_GetLastError();

extern bool Sys_InitTournamentLog(const char* fname);
extern void Sys_LogTournament(const char*);

#ifndef INFINITE
#define INFINITE     0xffff
#endif

#ifndef _WIN32
extern char* itoa(int, char*, int);
#endif

#endif
