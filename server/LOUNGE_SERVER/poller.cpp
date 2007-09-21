/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CPoller
  Created  : 04/16/2000

  OVERVIEW : Socket poller class.

 ****************************************************************************/

#include "poller.h"
#include "sys.h"
#include "pdu/pdu.h"
#include "pdu/pdunotify.h"
#include "lounge/lounge.h"
#include "loadbalancer/loadbalancer.h"
#include "accepter.h"
#include <string>
#ifndef _WIN32
#include <sys/param.h>
#endif


inline int Max(int x, int y)
{
  return x > y ? x : y;
}
CPoller* CPoller::pInst_ = NULL;

CPoller::CPoller(SOCKET sd_player, SOCKET sd_table)
  :
  sd_player_    (sd_player),
  max_player_sd_(sd_player + 1),
  sd_table_     (sd_table),
  max_table_sd_ (sd_table + 1)
{
  FD_ZERO(&player_readfds_);
  FD_ZERO(&table_readfds_);

  // Listen to new connections socket
  FD_SET(sd_player_, &player_readfds_);
  FD_SET(sd_table_, &table_readfds_);

  pInst_ = this;
}

CPoller::~CPoller()
{
  pInst_ = NULL;
}

int CPoller::poll()
{
  struct timeval wait;
  memset(&wait, 0, sizeof(wait));
  wait.tv_sec = 1;

  return poll(&wait);
}

/*
 * Handle incoming data on both player & table sockets.
 */
int CPoller::poll(struct timeval* timeout)
{
  fd_set readfds, exceptfds;
  memcpy(&readfds, &player_readfds_, sizeof(readfds));
  memcpy(&exceptfds, &player_readfds_, sizeof(exceptfds));

  SOCKET sdLB = CLoadBalancer::Inst() ?
                  CLoadBalancer::Inst()->getSocket() : INVALID_SOCKET;
  if (sdLB != INVALID_SOCKET)
  {
    max_player_sd_ = Max(sdLB + 1, max_player_sd_);
    FD_SET(sdLB, &readfds);
    FD_SET(sdLB, &exceptfds);
  }
  
  //
  // poll players & Load Balancer
  //
  int nready = ::select(max_player_sd_, &readfds, NULL, &exceptfds, timeout);
  int n = nready;

  if (nready == SOCKET_ERROR)
  {
    // This should never happen - it might happen
    // when we're shutting down. 
    static int m = 0;
    if (m == 0)
    {
	    Sys_LogError("select failed");
	    m = 1;
    }
    Sys_Sleep(1000);
  }

  if (nready != -1 && sdLB != INVALID_SOCKET && FD_ISSET(sdLB, &readfds))
  { // this table's socket has incoming data
    incomingPdu(sdLB, CLoadBalancer::Inst());
    FD_CLR((unsigned)sdLB, &readfds);
    n--;
  }

  if (nready != -1 && FD_ISSET(sd_player_, &readfds))
  {
    n--;
    //
    // new players socket fired
    //

    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    socklen_t clilen = sizeof(cliaddr);

    int sockfd = accept(sd_player_,
                        (struct sockaddr*)&cliaddr,
                        &clilen);

    if (sockfd != -1)
      CAccepter::Inst()->beginAccept(sockfd, &cliaddr);
  }

  // Get accepted new connections if any
  CAccepter::Inst()->getAccepted(max_player_sd_, &player_readfds_);

  for (PlayerIterator p = CLounge::Inst()->allPlayers();
       n > 0 && p.more();
       p.next())
  {
    CPlayer* player = p.current();

    if (player->getNetworkError())
    {
      // Player's connection has died
      char s[200];
      sprintf(s, "Network error (1), player %s.\n", player->getUsername());
      Sys_LogError(s);
      CLounge::Inst()->logout(player->getSocket(), player, true);
    }
    else
    {
      SOCKET sd = player->getSocket();
      if (sd == -1) continue;

      if (FD_ISSET(sd, &readfds))
      { // This player's socket has incoming data
        n--;
        incomingPdu(sd, player);
        FD_CLR((unsigned)sd, &readfds);
      }
            
      if (FD_ISSET(sd, &exceptfds))
      {
        // Player's connection has died
        char s[200];
        sprintf(s, "Network error (2), player %s.\n", player->getUsername());
        Sys_LogError(s);
        CLounge::Inst()->logout(player->getSocket(), player, true);
      }
    }
  }

  // Clean up completed logins
  CLounge::Inst()->postProcessLogins();

  //
  // poll tables
  //
  memcpy(&readfds, &table_readfds_, sizeof(readfds));
  timeout->tv_sec = 0;
  nready = ::select(max_table_sd_, &readfds, NULL, NULL, timeout);
  n = nready;

  if (FD_ISSET(sd_table_, &readfds))
  {
    n--;
    // 
    // new tables socket fired
    //
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    int clilen = sizeof(cliaddr);

    int sockd = Sys_accept(sd_table_,
                           (struct sockaddr*)&cliaddr,
                           &clilen);

    if (sockd != INVALID_SOCKET)
    {
      Sys_setnonblockio(sockd);

      string saddr = inet_ntoa(cliaddr.sin_addr);
      if (saddr == "127.0.0.1")
      { // the connection is from localhost, but we need to pass
        // the real ip address of this host to the players, not '127.0.0.1'

				struct hostent* phostent = NULL;
		    char hostname[MAXHOSTNAMELEN+1];
		    if (gethostname(hostname, MAXHOSTNAMELEN) == 0)
		      phostent = gethostbyname(hostname);
		    else 
		      phostent = gethostbyname("game2.pokerspot.com");

        if (phostent)
          memcpy(&cliaddr.sin_addr, phostent->h_addr_list[0], phostent->h_length);
      }

      u_long addr1 = cliaddr.sin_addr.s_addr;
      u_long addr = ntohl(addr1);
      if (CLounge::Inst()->addTable(sockd, addr))
      {
        // remember max_socket and save it to read fd_set
        max_table_sd_ = max(sockd + 1, max_table_sd_);
        FD_SET(sockd, &table_readfds_);

        printf("Accepted table connection from %s\n", inet_ntoa(cliaddr.sin_addr));
      }
      else
      {
        printf("Rejected table connection from %s\n", inet_ntoa(cliaddr.sin_addr));
      }
    }

#ifdef _WIN32
    if (CLounge::Inst()->numTables() == 8)
    {
      // Call listen again so the backlog will be
      // updated. Otherwise the 8-th table server's
      // connect will fail!
      int err = listen(sd_table_, SOMAXCONN);
      printf("Calling listen again: %i\n", err);
    }
#endif
  }

  for (TableIterator t = CLounge::Inst()->tables();
       n > 0 && t.more();
       t.next())
  {
    CTable* table = t.current();
    SOCKET sd = table->getSocket();
    if (sd != -1)
    {
      if (FD_ISSET(sd, &readfds))
      { // this table's socket has incoming data
        incomingPdu(sd, table);
        FD_CLR((unsigned)sd, &readfds);
        n--;
      }
    }
  }

/*
  //
  // poll Load Balancer & other Lounge Servers
  //
  if (CLoadBalancer::Inst())
  {
    SOCKET sd = CLoadBalancer::Inst()->getSocket();
    if (sd != INVALID_SOCKET)
    {
      fd_set lb;
      FD_ZERO(&lb);
      FD_SET(sd, &lb);

      memcpy(&readfds, &lb, sizeof(readfds));
      memcpy(&exceptfds, &lb, sizeof(exceptfds));
      timeout->tv_sec = 0;
      nready = ::select(1, &readfds, NULL, &exceptfds, timeout);
      n = nready;

      if (n > 0)
      {
        if (FD_ISSET(sd, &readfds))
        { // this table's socket has incoming data
          incomingPdu(sd, CLoadBalancer::Inst());
          FD_CLR((unsigned)sd, &readfds);
          n--;
        }
      }
    }
  }
*/

  return nready;
}


void CPoller::incomingPdu(SOCKET sd, CPlayer* player)
{
    int pdu_type = Cpdu::Peek(sd);
    if (pdu_type == -1)
    {
        // this connection has died 
        char s[200];
        sprintf(s, "Error reading socket %d (%s) - will terminate connection.\n", sd, player->getUsername());
        Sys_LogError(s);
        CLounge::Inst()->logout(sd, player, true);
    }
    else if (pdu_type != 0)
    {   //
        // the whole pdu has arrived - process it
        //

        Cpdu* pdu = Cpdu::Read(sd, pdu_type);

        if (pdu)
        {
            pdu->grok(sd, player);
            pdu = NULL;
        }
    }
    else
    {
        // the whole pdu has not arrived yet - leave
        // it to the next iteration
    }
}


void CPoller::incomingPdu(SOCKET sd, CTable* table)
{
  int pdu_type = Cpdu::Peek(sd);
  if (pdu_type == -1)
  {
    // this connection has died 
    printf("Error reading table socket %d - will terminate connection.\n", sd);
    CLounge::Inst()->tableLogout(sd, table);
  }
  else if (pdu_type != 0)
  { //
    // the whole pdu has arrived - process it
    //

    Cpdu* pdu = Cpdu::Read(sd, pdu_type);
    
    if (pdu)
    {
      pdu->grok(sd, table);
      pdu = NULL;
    }
  }
}

void CPoller::addPlayerSocket(SOCKET sd)
{
    if (sd != -1)
        FD_SET(sd, &player_readfds_);
}

void CPoller::removePlayerSocket(SOCKET sd)
{
    if (sd != -1)
        FD_CLR(sd, &player_readfds_);
}

void CPoller::addTableSocket(SOCKET sd)
{
    if (sd != -1)
        FD_SET(sd, &table_readfds_);
}

void CPoller::removeTableSocket(SOCKET sd)
{
    if (sd != -1)
        FD_CLR(sd, &table_readfds_);
}

void CPoller::incomingPdu(SOCKET sd, CLoadBalancer* lb)
{
  int pdu_type = Cpdu::Peek(sd);
  if (pdu_type == -1)
  {
    // this connection has died 
    printf("Error reading LoadBalancer socket %d.\n", sd);
    if (CLoadBalancer::Inst())
      CLoadBalancer::Inst()->closeConnection();
  }
  else if (pdu_type != 0)
  { //
    // the whole pdu has arrived - process it
    //

    Cpdu* pdu = Cpdu::Read(sd, pdu_type);
    
    if (pdu)
    {
      pdu->grok(sd, CLoadBalancer::Inst());
      pdu = NULL;
    }
  }
}