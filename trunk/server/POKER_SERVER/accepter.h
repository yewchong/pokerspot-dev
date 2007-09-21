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

    void beginAccept(SOCKET listensd);
    void accepted(SOCKET acceptedsd);

    int getAccepted(int* p_client,
                    int& r_maxfd,
                    int& r_maxi,
                    fd_set* p_allset);

    static CAccepter* Inst();

    void incThreads();
    void decThreads();

private:
    static CAccepter* pInst_;

    int          numThreads_;
    list<SOCKET> acceptedSockets_;
    Mutex        mutex_;
};

struct AcceptInfo
{
    SOCKET listensd;    
};


#endif