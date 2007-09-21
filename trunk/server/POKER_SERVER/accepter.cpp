#include "accepter.h"

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

    SOCKET sockfd = (SOCKET)(pV);

    try
    {
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));
        int clilen = sizeof(cliaddr);

        SOCKET newsd = Sys_accept_SSL(sockfd);

        if (newsd != INVALID_SOCKET)
        {
            printf("New connection accepted: sockfd: %d ipaddress: %s\n",
                   newsd, inet_ntoa(cliaddr.sin_addr));

            if (Sys_setnonblockio(newsd) < 0)
            {
              Sys_LogError("Sys_setnonblockio");
              Sys_close(newsd);
            }
            else
            {
              if (Sys_setnonagle(newsd) < 0)
              {
                Sys_LogError("Sys_setnonagle");
              }

              CAccepter::Inst()->accepted(newsd);
            }
        }    
    }
    catch(...)
    {
        Sys_LogError("Exception in accept thread!\n");
    }

    CAccepter::Inst()->decThreads();

#ifdef _POSIX_THREADS
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


void CAccepter::beginAccept(SOCKET sockfd)
{
    if (numThreads_ < 20)
    {
        Sys_CreateThread(AcceptProc, (void*)sockfd);
    }
    else
    {
        Sys_LogError("More than 20 accept threads\n");
        close(sockfd);
    }
}

void CAccepter::accepted(SOCKET newsd)
{
    SingleLock l(&mutex_);
    l.lock();

    acceptedSockets_.push_back(newsd);
}

int CAccepter::getAccepted(int* p_client,
                           int& r_maxfd,
                           int& r_maxi,
                           fd_set* p_allset)
{
    SingleLock l(&mutex_);
    l.lock();

    int numAccepted = 0;

    while (acceptedSockets_.size() > 0)
    {
        printf("Accepted one socket.\n");

        SOCKET socksd = *(acceptedSockets_.begin());
        acceptedSockets_.erase(acceptedSockets_.begin());

        // Save the new socket to client array
        int i = 0;
        for (i = 0; ((i < FD_SETSIZE) && (p_client[i] >= 0)); i++) ;   

        if (p_client[i] < 0)
            p_client[i] = socksd;
 
        FD_SET(socksd, p_allset);
        if (socksd > r_maxfd)
            r_maxfd = socksd;

        if (i > r_maxi)
            r_maxi = i;
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
