#ifndef __table_accepter_h__
#define __table_accepter_h__

#include <list>
#include "psi_win32.h"
#include "sync/sync.h"
#include "includes.h"
#include "compat.h"
USING_NS_T(std, list);


class CAccepter
{
public:
    CAccepter();

    void beginAccept(SOCKET listensd, struct sockaddr_in*);
    void accepted(SOCKET acceptedsd, const struct sockaddr_in&);

    int getAccepted(int& r_maxfd,
                    fd_set* p_allset);

    static CAccepter* Inst();

    void incThreads();
    void decThreads();

    struct AcceptItem
    {
        SOCKET sock_;
        struct sockaddr_in cliaddr_;

        AcceptItem(SOCKET s, const struct sockaddr_in& a)
            : sock_(s)
        {
            memcpy(&cliaddr_, &a, sizeof(struct sockaddr_in));
        }
        AcceptItem& operator=(const AcceptItem& rhs)
        {
            if (this != &rhs)
            {
                sock_ = rhs.sock_;
                memcpy(&cliaddr_, &rhs.cliaddr_, sizeof(struct sockaddr_in));
            }
            return *this;
        }
    };

private:
    static CAccepter* pInst_;

    int              numThreads_;
    list<AcceptItem> acceptedSockets_;
    Mutex            mutex_;
};

struct AcceptInfo
{
    SOCKET listensd;    
};


#endif