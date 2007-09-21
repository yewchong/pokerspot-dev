/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CAccepter
  Created  : 09/21/2001

  OVERVIEW : Class CAccepter implements doing SSL handshakes
             in separate threads.

 ****************************************************************************/

#include "accepter.h"
#include "poller.h"
#include "server.h"
#include "def.h"
#include "loadbalancer/loadbalancer.h"
#include "common/sys.h"
#include "common/errorlog.h"
#include <iostream.h>
#include <minmax.h>


//
// The actual work of accepting a new incoming connection
// is done in a separate thread. This is to avoid blocking
// the main thread on new connections.
//
// Incoming player connections are handled completely in the
// thread function. The function does the SSL handshake and
// then waits for the version data. Upon receiving the version
// data, the player is either redirected to a Lounge Server
// or the upgrade process is initiated.
//
THREADPROC_RETURN CDECL AcceptPlayerProc(VOID* pV)
{
  CAccepter::Inst()->incThreads();

  CAccepter::AcceptItem* pI = (CAccepter::AcceptItem*)pV;
  SOCKET sockfd = pI->sock_;

  SOCKET newsd = INVALID_SOCKET;

  try
  {
    newsd = Sys_accept_SSL(sockfd);

    if (newsd != INVALID_SOCKET)
    {
      CLB::Inst()->handlePlayerConnection(newsd);
    }
  }
  catch(...)
  {
    Err::Log() << "Exception in player accept thread!\n";
  }


  // The connection is completely handled by now

  if (newsd != INVALID_SOCKET)
  {
    // Allow some time for the data to get there
    Sys_Sleep(1000);
    Sys_closesocket(newsd);
  }

  delete pI;

  CAccepter::Inst()->decThreads();
}


//
// The accepted new Lounge Server socket is retrieved
// by the main thread by calling CAccepter::getAcceptedLounges.
//
THREADPROC_RETURN CDECL AcceptLoungeProc(VOID* pV)
{
  CAccepter::Inst()->incThreads();

  CAccepter::AcceptItem* pI = (CAccepter::AcceptItem*)pV;
  SOCKET sockfd = pI->sock_;

  try
  {
    SOCKET newsd = Sys_accept_SSL(sockfd);

    if (newsd != INVALID_SOCKET)
    {
      if (Sys_setnonblockio(newsd) < 0)
      {
        Err::Log() << "Sys_setnonblockio\n";
        Sys_closesocket(newsd);
      }
      else
      {
        if (Sys_setnonagle(newsd) < 0)
        {
          Err::Log() << "Sys_setnonagle\n";
        }

        CAccepter::Inst()->acceptedLounge(newsd, pI->cliaddr_);
      }
    }    
    else
    {
      Sys_closesocket(newsd);
    }
  }
  catch(...)
  {
    Err::Log() << "Exception in lounge accept thread!\n";
  }

  delete pI;

  CAccepter::Inst()->decThreads();
}


CAccepter* CAccepter::pInst_ = 0;
CAccepter g_Accepter;


CAccepter::CAccepter()
  :
  numThreads_(0),
  maxThreads_(CWG_DEFAULT_MAXTHREADS)
{
  pInst_ = this;
}

CAccepter* CAccepter::Inst()
{
  return pInst_;
}


void CAccepter::beginAcceptPlayer(SOCKET sockfd, struct sockaddr_in* addr)
{
  if (numThreads_ < maxThreads_)
  {
    AcceptItem* pI = new AcceptItem(sockfd, *addr);
    if (pI)
    {
      if (Sys_CreateThread(AcceptPlayerProc, (void*)pI) == -1)
      {
        Err::Log() << "Could not create accept thread.\n";
        ::closesocket(sockfd);
      }
    }
    else
    {
      Err::Log() << "CAccepter out of memory.\n";
      ::closesocket(sockfd);
    }
  }
  else
  {
    Err::Log() << "Too many accept threads: " << numThreads_ << CLog::nl;
    ::closesocket(sockfd);
  }
}

void CAccepter::beginAcceptLounge(SOCKET sockfd, struct sockaddr_in* addr)
{
  if (numThreads_ < maxThreads_)
  {
    AcceptItem* pI = new AcceptItem(sockfd, *addr);
    if (pI)
    {
      if (Sys_CreateThread(AcceptLoungeProc, (void*)pI) == -1)
      {
        Err::Log() << "Could not create accept thread.\n";
        ::closesocket(sockfd);
      }
    }
    else
    {
      Err::Log() << "CAccepter out of memory.\n";
      ::closesocket(sockfd);
    }
  }
  else
  {
    Err::Log() << "Too many accept threads.\n";
    ::closesocket(sockfd);
  }
}

void CAccepter::acceptedLounge(SOCKET newsd, const struct sockaddr_in& addr)
{
  SingleLock l(&mutex_);
  l.lock();
  acceptedSockets_.push_back(AcceptItem(newsd, addr));
}

int CAccepter::getAcceptedLounges(int& r_maxfd, fd_set* p_allset)
{
  SingleLock l(&mutex_);
  l.lock();
  
  int numAccepted = 0;
  
  while (acceptedSockets_.size() > 0)
  {
    AcceptItem item = *(acceptedSockets_.begin());
    acceptedSockets_.erase(acceptedSockets_.begin());
    int sockd = item.sock_;

    u_long addr = 0;
    string saddr = inet_ntoa(item.cliaddr_.sin_addr);
    if (saddr == "127.0.0.1")
    { // the connection is from localhost, but we need to pass
      // the real ip address of this host to the players, not '127.0.0.1'
      addr = CServer::Inst()->getLocalHostIpAddress();
    }
    else
    {
      u_long addr1 = item.cliaddr_.sin_addr.s_addr;
      addr = ntohl(addr1);
    }
    
    if (CLB::Inst()->addLounge(sockd, addr))
    {
      // remember max_socket and save it to read fd_set
      r_maxfd = max(sockd + 1, r_maxfd);
      FD_SET(sockd, p_allset);
      
      cout << "Accepted Lounge Server connection from " 
           << inet_ntoa(item.cliaddr_.sin_addr) << CLog::nl;
    }
  }
  
  return numAccepted;
}

void CAccepter::incThreads()
{
  SingleLock l(&mutex_);
  l.lock();
  
  numThreads_++;  
  cout << "Accepter thread started: " << numThreads_ << CLog::nl;
}

void CAccepter::decThreads()
{
  SingleLock l(&mutex_);
  l.lock();
  
  cout << "Accepter thread finished: " << numThreads_ << CLog::nl;
  numThreads_--;
}
