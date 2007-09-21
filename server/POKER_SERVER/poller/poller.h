#ifndef __poller_h__
#define __poller_h__

#include "../includes.h"

#define DEFAULT_TABLE_PORT	    8101
#define LISTENQUEUE				10

class CTable;
class Cpdu;
class CLoungeServer;

extern fd_set allset, rset;

class CPoller
{
public:
	CPoller();
    ~CPoller();
    static CPoller* Instance();
	int  init(int argc, char* argv[],
              struct sockaddr_in*, CTable*,
              bool ssl = false);
	void newClient();
	void poll();
	void poll(struct timeval *);
    void removeClient(SOCKET);
    void shutdown();
    void sendTableLogout();
private:
	int client_[FD_SETSIZE], listenfd_, maxi_, maxfd_;
	struct sockaddr_in* psaServAddr_;
	CTable* pTable_;
    CLoungeServer* pLounge_;

    static CPoller* pInstance_;
};

inline CPoller* CPoller::Instance()
{
    return pInstance_;
}

#endif
