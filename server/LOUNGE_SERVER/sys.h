#ifndef __psi_sys_h__
#define __psi_sys_h__

#include "includes.h"
#include "compat.h"

#define LOG_FLAG_SOCKET     1
#define LOG_FLAG_DATABASE   2
#define LOG_FLAG_ALL        0xff
bool Sys_InitErrorLog(const char* fname);
extern void Sys_LogError(const char* msg, int flag = 0);
extern void Sys_PrintError(const char* msg);

extern const char* Sys_GetDate();

extern bool Sys_InitSocket();
extern void Sys_ShutdownSocket();
extern void Sys_CloseSocket(SOCKET);

// Socket routines
extern void Sys_EnableSSL();
extern void Sys_DisableSSL();
extern bool Sys_IsSSLEnabled();
extern int Sys_setnonblockio(int sockfd);
extern int Sys_setnonagle(int sockfd);
extern int Sys_accept(int listenfd, struct sockaddr* cliaddr, int* addrlen);
extern int Sys_peek(int sockfd, char* buffer, int nbytes);
extern int Sys_recv(int sockfd, char* buffer, int nbytes);
extern int Sys_send(int sockfd, const char* buffer, int nbytes);
extern void Sys_alarm(int);

extern int Sys_accept_SSL(int);

extern bool Sys_InitTournamentLog(const char* fname);
extern void Sys_LogTournament(const char*, bool nl = true);

// Thread creation/ending 
extern int Sys_CreateThread(ThreadFunc, void* args);
extern void Sys_EndThread();

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


// Lounge server
extern SOCKET Sys_SpawnTableMaster(u_long addr,
                                   u_short port);



#endif