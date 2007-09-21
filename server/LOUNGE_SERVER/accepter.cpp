#include "accepter.h"
#include "pdu/pdunotify.h"
#include "lounge/lounge.h"
#include "poller.h"
#include "sys.h"

//
// The actual work of accepting a new incoming connection
// is done in a separate thread. This is to avoid blocking
// the main thread on new connections.
//
// The accepted new socket is retrieved by the main thread
// by calling CAccepter::getAccepted.
//
THREADPROC_RETURN CDECL AcceptProc(VOID* pV)
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
              Sys_LogError("Sys_setnonblockio");
              Sys_CloseSocket(newsd);
            }
            else
            {
              if (Sys_setnonagle(newsd) < 0)
              {
                Sys_LogError("Sys_setnonagle");
              }

              CAccepter::Inst()->accepted(newsd, pI->cliaddr_);
            }
        }    
    }
    catch(...)
    {
        Sys_LogError("Exception in accept thread!\n");
    }

    delete pI;

    CAccepter::Inst()->decThreads();

#ifndef _WIN32
    return (THREADPROC_RETURN)0;
#endif
}

CAccepter* CAccepter::pInst_ = 0;
CAccepter g_Accepter;

CAccepter::CAccepter()
    :
    numThreads_(0)
{
    pInst_ = this;
}

CAccepter* CAccepter::Inst()
{
    return pInst_;
}


void CAccepter::beginAccept(SOCKET sockfd, struct sockaddr_in* addr)
{
    if (numThreads_ < 20)
    {
        AcceptItem* pI = new AcceptItem(sockfd, *addr);
        if (pI)
        {
            Sys_CreateThread(AcceptProc, (void*)pI);
        }
        else
        {
            Sys_LogError("CAccepter out of memory\n");
            close(sockfd);
        }
    }
    else
    {
        Sys_LogError("More than 20 accept threads\n");
        close(sockfd);
    }
}

void CAccepter::accepted(SOCKET newsd, const struct sockaddr_in& addr)
{
    SingleLock l(&mutex_);
    l.lock();

    acceptedSockets_.push_back(AcceptItem(newsd, addr));
}

int CAccepter::getAccepted(int& r_maxfd,
                           fd_set* p_allset)
{
    SingleLock l(&mutex_);
    l.lock();

    int numAccepted = 0;

    while (acceptedSockets_.size() > 0)
    {
        printf("Accepted one socket.\n");

        AcceptItem item = *(acceptedSockets_.begin());
        acceptedSockets_.erase(acceptedSockets_.begin());
        int sockd = item.sock_;

        if (CLounge::Inst()->numPlayers() >= CLounge::Inst()->getMaxPlayers())
        {
            CpduNotify pdu;
            pdu.sendNotifyMessage(sockd,
                                  "The server is full - please try connecting later.");
            CPoller::Inst()->removePlayerSocket(sockd);
            Sys_CloseSocket(sockd);
        }
        else
        {
            char keepalives = 1;
            setsockopt(sockd, SOL_SOCKET, SO_KEEPALIVE, &keepalives, sizeof(keepalives));

            if (CLounge::Inst()->addPlayer(sockd, &item.cliaddr_.sin_addr))
            {
                // remember max_socket and save it to read fd_set
                r_maxfd = max(sockd + 1, r_maxfd);
                FD_SET(sockd, p_allset);
   
                printf("Accepted player connection from %s\n", inet_ntoa(item.cliaddr_.sin_addr));
            }
        }
    }

    return numAccepted;
}

void CAccepter::incThreads()
{
    SingleLock l(&mutex_);
    l.lock();

    numThreads_++;

    printf("Accepter thread started: %i\n", numThreads_);
}

void CAccepter::decThreads()
{
    SingleLock l(&mutex_);
    l.lock();

    printf("Accepter thread finished: %i\n", numThreads_);
    numThreads_--;
}
