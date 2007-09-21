#include "poller.h"
#include "def.h"
#include "sys.h"
#include "ssl_funcs.h"
#include "accepter.h"
#include "nbiobuf.h"
#include "table/table.h"
#include "pduhandler/pduhandler.h"
#include "lounge/loungeserver.h"


CPoller* CPoller::pInstance_ = NULL;

CPoller::CPoller()
{
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        client_[i] = -1;
    }

    listenfd_ = 0;
    maxi_ = 0;
    maxfd_ = 0;
    psaServAddr_ = NULL;
    pTable_ = NULL;
    pLounge_ = NULL;

    pInstance_ = this;
};

CPoller::~CPoller()
{
    pInstance_ = NULL;
}

//
// Initialize sockets API, SSL and start listening socket.
// Return -1 on error, 0 on success.
//
int CPoller::init(int argc, char* argv[], struct sockaddr_in *psaGlobal, CTable *pCTable, bool ssl)
{   
    psaServAddr_ = psaGlobal;
    pTable_ = pCTable;

    if (!Sys_InitSocket())
    {
        Sys_LogError("InitSockets failed");
        return -1;
    }

    // Initialize SSL
    if (ssl)
        Sys_EnableSSL();
    else
        Sys_DisableSSL();

    if (Sys_IsSSLEnabled() && init_ssl() == -1)
    {
        Sys_LogError("Can't initialize SSL");
        return -1;
    }


    // Bind, then Listen on port

    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, NULL, NULL);

    // Bind to port
    if ((bind(listenfd_, (struct sockaddr *)psaServAddr_, sizeof(sockaddr_in))) < 0)
    {
        Sys_close(listenfd_);
        Sys_LogError("error on bind:");
        return -1;
    }

    // Listen on port
    if ((listen(listenfd_, LISTENQUEUE)) < 0)
    {
        Sys_close(listenfd_);
        Sys_LogError("error on listen:");
        return -1;
    }

    if (CWG_TABLEHANDSHAKE_TIMEOUT > 0)
    {
      int ssl_handshake_timeout = CWG_TABLEHANDSHAKE_TIMEOUT;
      int err = setsockopt(listenfd_,
                       SOL_SOCKET,
                       SO_RCVTIMEO,
                       (const char*)&ssl_handshake_timeout,
                       sizeof(ssl_handshake_timeout));

      if (err == -1)
      {
        Sys_LogError("setsockopt SO_RCVTIMEO fails");
      }
    }

    if (argc >= 9)
    {
        printf("Connecting to Lounge...");
        pLounge_ = CLoungeServer::ConnectToLounge(atoi(argv[6]),        // host addr
                                                  htons(atoi(argv[7])));// port

        if (!pLounge_)
        {
            Sys_LogError("Can't connect to lounge");
            Sys_close(listenfd_);
            return -1;
        }
        else
        {
            printf(" ok, logging in.\n");
            pLounge_->sendTableLogin(atoi(argv[1]), // queue index
                                     atoi(argv[2]), // table number
                                     atoi(argv[4]), // table lo
                                     atoi(argv[5]), // table hi
                                     ntohs(psaGlobal->sin_port));
        }
    }

    for (int i = 0; i < FD_SETSIZE; i++)
        client_[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd_, &allset);

    if (pLounge_)
    {
        maxfd_ = max((int)listenfd_, (int)pLounge_->getSocket());
        client_[0] = pLounge_->getSocket();
        FD_SET(pLounge_->getSocket(), &allset);
        maxi_ = 0;
    }
    else
    {
        maxfd_ = listenfd_;
        maxi_ = -1;
    }

    if (DEBUG & DEBUG_POLLER)
        printf("FD_SETSIZE=%d\n", FD_SETSIZE);

    return 0;
};


void CPoller::poll()
{
    struct timeval *pdu_wait;

    pdu_wait = (struct timeval *)malloc(sizeof(struct timeval));
    bzero(pdu_wait, sizeof(struct timeval));

    pdu_wait->tv_sec = 1;

    poll(pdu_wait);

    return;
};


void CPoller::poll(struct timeval *timeout)
{
    int i = 0, nready = 0, sockfd = 0;

    memcpy(&rset, &allset, sizeof(fd_set));
    fd_set exceptfds;
    memcpy(&exceptfds, &rset, sizeof(fd_set));

    nready = select(maxfd_ + 1, &rset, NULL, &exceptfds, timeout);

    if (nready == -1)
    {
        char s[200];
#ifdef _WIN32
        sprintf(s, "select fails: %i\n", WSAGetLastError());
#else
        sprintf(s, "select fails: %i\n", errno);
#endif
        Sys_LogError(s);
    }

    // Keep track of how many fired sockets we have processed.
    // This will allow early exit if e.g., only the first socket has fired.
    int nfired = nready;

    if (nfired > 0 && FD_ISSET(listenfd_, &rset))
    {
        nfired--;

        // New connection, the SSL handshake for the new
        // connection is done by the CAccepter class

        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));
        int clilen = sizeof(cliaddr);

        int sockfd = accept(listenfd_,
                            (struct sockaddr*)&cliaddr,
                            (socklen_t*)&clilen);

        if (sockfd != -1)
            CAccepter::Inst()->beginAccept(sockfd);
    }

    // Get accepted new connections if any
    CAccepter::Inst()->getAccepted(client_, maxfd_, maxi_, &allset);


    for (i = 0; (i <= maxi_) && (nfired > 0); i++)
    {
        sockfd = client_[i];
        if (sockfd != -1) // XXX: was: sockfd > 0!!!
        {
            if (FD_ISSET(sockfd, &rset))
            {
                nfired--;

                int pdu_type = Cpdu::Peek(sockfd);

                if (pdu_type == -1)
                {
                    // this connection has died 
                    if (CLoungeServer::Inst() && sockfd == CLoungeServer::Inst()->getSocket())
                    {   // Connection to the lounge server has died!
                        Sys_LogError("Lounge server is disconnected.\n");
                        CLoungeServer::Inst()->closeConnection();
                        removeClient(sockfd);
                    }
                    else
                    {   // Connection to a player has died
		        static int c = 50;
			if (c > 0)
			  {
			    c--;
			    char buf[200];
			    sprintf(buf, "Network error(1) socket %d\n", sockfd);
			    Sys_LogError(buf);
			  }

                        pTable_->disconnectPlayer(sockfd);
                    }
                }
                else if (pdu_type != 0)
                {
                    //
                    // the whole pdu has arrived - process it
                    //
		            Cpdu* pdu = Cpdu::Read(sockfd, pdu_type);

                    if (pdu)
                    {
                        pdu->grok(sockfd, pTable_);
                        pdu = NULL;
                        FD_CLR((unsigned)sockfd, &rset);
                    }
                }
                else
                {
                    // the whole pdu has not arrived yet - leave
                    // it to the next iteration
                }
            }
        }
    }

    return;
};

void CPoller::sendTableLogout()
{
    if (pLounge_)
        pLounge_->sendTableLogout();
}

void CPoller::shutdown()
{
    if (pLounge_)
    {
        pLounge_->closeConnection();
        delete pLounge_;
        pLounge_ = NULL;
    }

    for (int i = 0; i < (int)NUMOF(client_); i++)
    {
        if (client_[i] != -1)
        {
            Sys_close(client_[i]);
            client_[i] = -1;
        }
    }

    close(listenfd_);
    listenfd_ = 0;

    Sys_ShutdownSocket();
}

void CPoller::removeClient(SOCKET sd)
{
    if (sd == -1) return; // closed already 

    for (int i = 0; i < (int)NUMOF(client_); i++)
    {
        if (client_[i] == sd)
        {
            client_[i] = -1;
        }
    }

    FD_CLR(sd, &allset);
    FD_CLR(sd, &rset);

    Sys_close(sd);

    // XXX should update maxi_ & maxfd_?
}
