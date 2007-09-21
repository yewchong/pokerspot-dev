/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : sys.h
  Created  : 09/21/2000

  OVERVIEW : Collection of miscellaneous functions.

 ****************************************************************************/


#include "common/sys.h"
#include "common/errorlog.h"
#include "common/ssl_funcs.h"
#include <openssl/ssl.h>
#include <process.h>
#include <string>
USING_NS_T(std, string);


namespace
{
  BOOL g_bSSLEnabled = TRUE;
}

BOOL Sys_SSLEnabled()
{
  return g_bSSLEnabled;
}


void Sys_SetSSLEnabled(BOOL b)
{
  g_bSSLEnabled = b;
}

//
// Socket functions
//
int Sys_InitSocket()
{
  int rc = 0;
  
#ifdef _WIN32
  WSADATA wsaData;
  WORD wVersionRequested;
  int err;
  
  // Ask for Winsock 1.1 functionality
  wVersionRequested = MAKEWORD(1, 1);
  err = ::WSAStartup( wVersionRequested, &wsaData );
  if (err != 0)
  {
    Err::Log() << "Error " << err << ": Winsock not available.";
    rc = -1;
  }
#endif

  if (Sys_SSLEnabled())
    init_ssl();

  return rc;
}

int Sys_ShutdownSocket()
{
#ifdef _WIN32
  ::WSACleanup();
#endif

  if (Sys_SSLEnabled())
    ssl_shutdown();

  return 0;
}

int Sys_setnonagle(SOCKET sockfd)
{
  BOOL no_nagle_algorithm = TRUE;
  return setsockopt(sockfd,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    (char*)&no_nagle_algorithm,
                    sizeof(no_nagle_algorithm));
}

int Sys_setnonblockio(SOCKET sockfd)
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

int Sys_accept_SSL(SOCKET sockfd)
{
  if (sockfd != INVALID_SOCKET)
  {
    if (Sys_SSLEnabled())
    {
      int rc = ssl_accept(sockfd);
      if (rc == -1)
        return -1;
    }
  }
  
  return sockfd;
}

int Sys_peek(SOCKET sockfd, char* buffer, int nbytes)
{
  if (Sys_SSLEnabled())
  {
    return ssl_peek(sockfd, buffer, nbytes);
  }
  else
  {
    return recv(sockfd, buffer, nbytes, MSG_PEEK);
  }
}

int Sys_send(SOCKET sockfd, const char* buffer, int nbytes)
{
  if (Sys_SSLEnabled())
  {
    return ssl_write(sockfd, buffer, nbytes);
  }
  else
  {
    return send(sockfd, buffer, nbytes, 0);
  }
}

int Sys_recv(SOCKET sockfd, char* buffer, int nbytes)
{
  if (Sys_SSLEnabled())
  {
    return ssl_read(sockfd, buffer, nbytes);
  }
  else
  {
    return recv(sockfd, buffer, nbytes, 0);
  }
}

int Sys_closesocket(SOCKET s)
{
  if (Sys_SSLEnabled())
  {
    ssl_close(s);
  }
  else
  {
#ifdef _WIN32
    ::closesocket(s);
#else
    ::close(s);
#endif
  }

  return 0;
}

int Sys_ErrIsWouldBlock()
{
#ifdef _WIN32
  return (WSAGetLastError() == WSAEWOULDBLOCK);
#else
  return (errno == EAGAIN);
#endif
}

BOOL Sys_MakeSockAddr(const char* host, short port, struct sockaddr* pResult)
{
  if (host == NULL || *host == '\0')
    return FALSE;

  struct hostent* pHostEnt = 0;
  struct sockaddr_in address;

  memset(pResult, 0, sizeof(*pResult));
  memset(&address, 0, sizeof(address));

  pHostEnt = gethostbyname(host);

  if (!pHostEnt)
  { // try with numeric address
    u_long res = inet_addr(host);
    if (res != INADDR_NONE)
      pHostEnt = gethostbyaddr((char*)&res, 4, PF_INET);
  }

  if (pHostEnt != NULL)
  {
    address.sin_family = AF_INET;
    address.sin_port   = port;
    memcpy(&address.sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
    memcpy(pResult, &address, sizeof(address));
  }

  return pHostEnt != NULL;
}

SOCKET Sys_MakeConnection(UINT ipaddr, short port)
{
  SOCKET sd = INVALID_SOCKET;
  struct sockaddr sockAddr;

  struct in_addr inaddr;
  inaddr.s_addr = ipaddr;
  char* p = inet_ntoa(inaddr);
  if (p == NULL)
    return INVALID_SOCKET;
  string host = p;

  if (ipaddr == 0 || strcmp(p, "127.0.0.1") == 0)
  { // special case to ease local testing
    host = "localhost";
  }

  if (Sys_MakeSockAddr(p, port, &sockAddr))
  {
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd != INVALID_SOCKET)
    {
      int rc = connect(sd, &sockAddr, sizeof(sockAddr));
      if (rc >= 0)
      {
        if (Sys_SSLEnabled())
          rc = ssl_connect(sd);

        if (rc < 0)
        {
#ifdef ERROR_LOGGING_
          Err::Log() << "ssl_connect fails: " << Sys_GetLastError() << CLog::nl;
#endif
          ssl_close(sd);
          sd = INVALID_SOCKET;
        }
      }
      else
      {
#ifdef ERROR_LOGGING_
        Err::Log() << "connect fails: " << Sys_GetLastError() << CLog::nl;
#endif
        closesocket(sd);
        sd = INVALID_SOCKET;
      }
    }
  }

  return sd;
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


const char* Sys_GetDate()
{
  time_t ltime;
  time(&ltime); 
  return ctime(&ltime);
}

DWORD Sys_GetLastError()
{
#ifdef _WIN32
  return GetLastError();
#else
  return ::errno;
#endif
}

DWORD Sys_GetLastSocketError()
{
#ifdef _WIN32
  return WSAGetLastError();
#else
  return ::errno;
#endif
}


