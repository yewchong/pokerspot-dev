/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : sslfuncs.h
  Created  : 04/24/2000

  OVERVIEW : SSL functions for storing and retrieving socket -> SSL
             associations.

             <socket,SSL*> pairs are stored into a hash table, lookup
             is constant time.

 ****************************************************************************/

#include "sys.h"
#include "sync/sync.h"
#include "def.h"
#include "compat.h"
#include "includes.h"
#include "ssl_funcs.h"
#include <process.h>
#include <string>
USING_NS_T(std, string);


#ifdef _WIN32
VOID CALLBACK AlarmHandler(HWND, UINT, UINT, DWORD);
#endif

#ifndef _WIN32
// itoa(int from, char *to, int radix) is not in POSIX stdlib
// added here for FreeBSD build -- 4/22/00
char *itoa(int from, char *to, int radix)
{
  sprintf(to, "%i", from);
  return to;
};
#endif


// 
// SSL enable/disable. By default SSL will be off!
//
static bool g_enableSSL = false;

void Sys_EnableSSL()
{
    g_enableSSL = true;
}

void Sys_DisableSSL()
{
    g_enableSSL = false;
}

bool Sys_IsSSLEnabled()
{
    return g_enableSSL;
}

int Sys_setnonblockio(int sockfd)
{
#ifdef _WIN32
    u_long block = 1;
    return ioctlsocket(sockfd, FIONBIO, &block);
#else
    int arg = 0;
    if (fcntl(sockfd, F_GETFL, &arg) < 0)
    {
        Sys_LogError("fcntl");
    }
    return fcntl(sockfd, F_SETFL, arg | O_NONBLOCK);
#endif
}

int Sys_setnonagle(int sockfd)
{
  int no_nagle_algorithm = 1;
  return setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&no_nagle_algorithm, sizeof(no_nagle_algorithm));
}

int Sys_peek(int sockfd, char* buffer, int nbytes)
{
    if (g_enableSSL)
    {
        return ssl_peek(sockfd, buffer, nbytes);
    }
    else
    {
        return recv(sockfd, buffer, nbytes, MSG_PEEK);
    }
}

int Sys_pending(int sockfd)
{
    if (g_enableSSL)
    {
        return ssl_pending(sockfd);
    }
    else
    {
        return -1;//recv(sockfd, buffer, nbytes, MSG_PEEK);
    }
}


int Sys_recv(int sockfd, char* buffer, int nbytes)
{
    if (g_enableSSL)
    {
        return ssl_read(sockfd, buffer, nbytes);
    }
    else
    {
        return recv(sockfd, buffer, nbytes, 0);
    }
}

int Sys_send(int sockfd, const char* buffer, int nbytes)
{
    if (g_enableSSL)
    {
        return ssl_write(sockfd, buffer, nbytes);
    }
    else
    {
        return send(sockfd, buffer, nbytes, 0);
    }
}

int Sys_accept(int listenfd, struct sockaddr* cliaddr, int* addrlen)
{
    int sockfd = accept(listenfd, cliaddr, (socklen_t*)addrlen);

    if (sockfd != INVALID_SOCKET)
    {
        if (g_enableSSL)
        {
            int rc = ssl_accept(sockfd);
            if (rc == -1)
            {   // SSL problem
                close(sockfd);
                return -1;
            }
        }
    }

    return sockfd;
}

int Sys_accept_SSL(int sockfd)
{
    if (sockfd != INVALID_SOCKET)
    {
        if (g_enableSSL)
        {
            int rc = ssl_accept(sockfd);
            if (rc == -1)
            {   // SSL problem
                close(sockfd);
                return -1;
            }
        }
    }

    return sockfd;
}


void Sys_close(int sd)
{
    if (g_enableSSL)
    {
        ssl_close(sd);
    }
    else
    {
        close(sd);
    }
}


void AlarmHandler(int)
{
    // Doing nothing should fail the blocking
    // accept with an EINTR
    return;
}

void Sys_alarm(int a)
{
#ifndef _WIN32
    alarm(a);
#endif
}


void Sys_PrintError(const char* msg, DWORD lastError)
{
#ifdef _WIN32
    printf(msg);
    printf(" ");
    printf("Last socket error: %d\n", lastError);
#else
    perror(msg);
#endif
}

static string g_chiplog_file("chiplog.txt");

bool Sys_InitChipLog(const char* fname)
{
  g_chiplog_file = fname;
  return true;
}

void Sys_LogChipBuyin(const char* player,
                      double totalChips,
                      double buyinChips)
{
    FILE* fp = fopen(g_chiplog_file.c_str(), "a+");
    if (fp)
    {      
        fprintf(fp, Sys_GetDate());
        fprintf(fp, "%s buyin %9.2f total %9.2f\n\n",
                player, buyinChips, totalChips);
        fclose(fp);
        fp = NULL;
    }
}

void Sys_LogChipSave(const char* player,
                     double totalChips,
                     double saveChips)
{
    FILE* fp = fopen(g_chiplog_file.c_str(), "a+");
    if (fp)
    {      
        fprintf(fp, Sys_GetDate());
        fprintf(fp, "%s save %9.2f total %9.2f\n\n",
                player, saveChips, totalChips);
        fclose(fp);
        fp = NULL;
    }
}


static string g_errlog_file("errorlog.txt");

bool Sys_InitErrorLog(const char* fname)
{
    g_errlog_file = fname;
    return true;
}


void Sys_LogError(const char* msg)
{
    int lastErr = Sys_GetLastError();

    FILE* fp = fopen(g_errlog_file.c_str(), "a+");
    if (fp)
    {      
        fprintf(fp, Sys_GetDate());
        fprintf(fp, msg);
#ifdef _WIN32
        fprintf(fp, "   Last socket error: %d\n", lastErr);
#endif
        fprintf(fp, "\n");

        fclose(fp);
        fp = NULL;
    }

    // print it to stdout as well
    Sys_PrintError(msg, lastErr);
}


static string g_tlog_file("tournamentlog.txt");

bool Sys_InitTournamentLog(const char* fname)
{
    g_tlog_file = fname;
    return true;
}

// Protect with a mutex
//Mutex g_sys_mutex2;

void Sys_LogTournament(const char* msg)
{
    //SingleLock l(&g_sys_mutex2);
    //l.lock();

    FILE* fp = fopen(g_tlog_file.c_str(), "a+");
    if (fp)
    {      
        fprintf(fp, Sys_GetDate());
        fprintf(fp, msg);
        fprintf(fp, "\n");

        fclose(fp);
        fp = NULL;
    }

    // print it to stdout as well
    Sys_PrintError(msg, 0);
}

const char* Sys_GetDate()
{
  time_t ltime;
  time(&ltime); 
  return ctime(&ltime);
}

bool Sys_InitSocket()
{
    bool rc = true;

#ifdef _WIN32

    WSADATA wsaData;
    WORD wVersionRequested;
    int err;

    /* Ask for Winsock 1.1 functionality */
    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if (err != 0)
    {
      printf("Error %d: Winsock not available\n", err);
      rc = false;
    }

#endif

    return rc;
}

void Sys_ShutdownSocket()
{
#ifdef _WIN32
  WSACleanup();
#endif
}


// 
// WIN32 API implementation of thread routines.
//
int Sys_CreateThread(ThreadFunc pThreadFunc, void* pArgs)
{
  return ::_beginthread(pThreadFunc, 0, pArgs);
}

void Sys_EndThread()
{
  ::_endthread();
}

//
// Event/Condition
// 
HANDLE Sys_CreateEvent()
{
  return ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

int Sys_WaitEvent(HANDLE h, DWORD dwTimeout)
{
  // WIN32 has same implementation for events & mutexes
  return Sys_LockMutex(h, dwTimeout);
}

int Sys_SetEvent(HANDLE h)
{
  return ::SetEvent(h);
}

int Sys_PulseEvent(HANDLE h)
{
  return ::PulseEvent(h);
}

int Sys_ResetEvent(HANDLE h)
{
  return ::ResetEvent(h);
}

void Sys_DestroyEvent(HANDLE h)
{
  ::CloseHandle(h);
}

//
// Mutex
//
HANDLE Sys_CreateMutex()
{
  return ::CreateMutex(NULL, FALSE, NULL);
}

int Sys_LockMutex(HANDLE h, DWORD timeout)
{
  if (::WaitForSingleObject(h, timeout) == WAIT_OBJECT_0)
    return 1;
  else
    return 0;
}

int Sys_ReleaseMutex(HANDLE h)
{
  return ::ReleaseMutex(h);
}

int Sys_DestroyMutex(HANDLE h)
{
  return ::CloseHandle(h);
}



//
// Misc
//
void Sys_Sleep(DWORD msecs)
{
#ifdef _WIN32
  ::Sleep(msecs);
#else
  sleep(msecs / 1000);
#endif // _WIN32
}


int Sys_GetLastError()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
