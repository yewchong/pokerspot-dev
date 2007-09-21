#include "sys.h"
#include "def.h"
#include "compat.h"
#include "includes.h"
#include <openssl/ssl.h>
#include "ssl_funcs.h"
#include <process.h>
#include <string>
USING_NS_T(std, string);


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
    fcntl(sockfd, F_GETFL, &arg);
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

// This socket is used to break blocking SSL accept
// if it does not complete in time.
int g_AcceptSock = -1;

int Sys_accept(int listenfd, struct sockaddr* cliaddr, int* addrlen)
{
    int sockfd = accept(listenfd, cliaddr, (socklen_t*)addrlen);

    if (sockfd != INVALID_SOCKET)
    {
        if (g_enableSSL)
        {
            g_AcceptSock = sockfd;
            int rc = ssl_accept(sockfd);
            g_AcceptSock = 0;

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

// Closing the socket on which we're doing
// the blocking SSL_accept will abort the
// accept and return an error.
void AlarmHandler(int)
{
    if (g_AcceptSock != -1)
    {
        close(g_AcceptSock);
        g_AcceptSock = -1;
    }
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
    printf(" Last socket error: %d\n", lastError);
#else
    perror(msg);
#endif
}

static string g_errlog_file("errorlog-lounge.txt");

bool Sys_InitErrorLog(const char* fname)
{
    g_errlog_file = fname;
    return true;
}

void Sys_LogError(const char* msg, int flags)
{
#ifdef _WIN32
    int lastError = WSAGetLastError();
#else
    int lastError = errno;
#endif

    FILE* fp = fopen(g_errlog_file.c_str(), "a+");
    if (fp)
    {
        fprintf(fp, Sys_GetDate());
        fprintf(fp, msg);

        fprintf(fp, "   Last socket error: %d\n", lastError);

        fclose(fp);
    }
    fp = NULL;

    // print it to stdout as well
    Sys_PrintError(msg, lastError);
}

static string g_tlog_file("tournamentlog.txt");

bool Sys_InitTournamentLog(const char* fname)
{
    g_tlog_file = fname;
    return true;
}


void Sys_LogTournament(const char* msg, bool nl)
{
    FILE* fp = fopen(g_tlog_file.c_str(), "a+");
    if (fp)
    {
        if (nl)
            fprintf(fp, Sys_GetDate());
        fprintf(fp, msg);
        if (nl)
            fprintf(fp, "\n");

        fclose(fp);
        fp = NULL;
    }

    // print it to stdout as well
    printf(msg);
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

void Sys_CloseSocket(SOCKET s)
{
    if (g_enableSSL)
    {
        ssl_close(s);
    }
    else
    {
        close(s);
    }
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
#endif
}


SOCKET Sys_SpawnTableMaster(u_long addr,
                            u_short port)
{
#ifdef _WIN32
    // In Windows the master is run in the same machine
    char buf[128];
    sprintf(buf, "%ld %hd", addr, port);
    int rc = (int)ShellExecute(NULL, NULL, "table_master", buf, NULL, 0);
#else
    // In *nix do the real thing
#endif
    return 0;
}
