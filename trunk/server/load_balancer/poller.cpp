/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPoller
  Created  : 09/21/2001

  OVERVIEW : This class polls sockets for incoming data.

 ****************************************************************************/


#include "poller.h"
#include "accepter.h"
#include "pdu/pdu.h"
#include "loadbalancer/loadbalancer.h"
#include "common/errorlog.h"
#include <time.h>
#include <minmax.h>
#include <string>
#include <iostream.h>
USING_NS_T(std, string);


CPoller* CPoller::pInst_ = NULL;


CPoller::CPoller(SOCKET sd_player, SOCKET sd_lounge)
  :
  sd_player_(sd_player),
  sd_lounge_(sd_lounge),
  max_listen_sd_(max((sd_lounge + 1), (sd_player + 1)))
{
  FD_ZERO(&listensds_);

  // Listen to new connection sockets
  FD_SET(sd_player_, &listensds_);
  FD_SET(sd_lounge_, &listensds_);

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
  wait.tv_sec = 5;

  return poll(&wait);
}

/*
 * Handle incoming data on both player & lounge sockets.
 */
int CPoller::poll(struct timeval* timeout)
{
  fd_set readfds, exceptfds;
  memcpy(&readfds, &listensds_, sizeof(readfds));
  memcpy(&exceptfds, &listensds_, sizeof(exceptfds));

  //
  // sockets for incoming data
  //
  int nready = ::select(max_listen_sd_, &readfds, NULL, &exceptfds, timeout);
  int n = nready;

  if (nready == SOCKET_ERROR)
  {
    Err::Log() << "select failed: " << Sys_GetLastError() << CLog::nl;
    Sys_Sleep(1000);
  }

  if (FD_ISSET(sd_player_, &readfds))
  {
    n--;
    newPlayerConnection(sd_player_);
  }


  if (FD_ISSET(sd_lounge_, &readfds))
  {
    n--;
    newLoungeConnection(sd_lounge_);
  }

  // Get new Lounge connections if any
  CAccepter::Inst()->getAcceptedLounges(max_listen_sd_, &listensds_);

  for (LoungeIterator li = CLB::Inst()->getLounges();
       n > 0 && li.more();
       li.next())
  {
    CLounge* lounge = li.current();
    SOCKET sd = lounge->getSocket();
    if (sd != INVALID_SOCKET)
    {
      if (FD_ISSET(sd, &readfds))
      { // this lounge's socket has incoming data
        incomingPdu(sd, lounge);
        FD_CLR((unsigned)sd, &readfds);
        n--;
      }
    }
  }

  long now = time(NULL);
  CLB::Inst()->processLogouts(now);

  return nready;
}


void CPoller::incomingPdu(SOCKET sd, CLounge* lounge)
{
  int pdu_type = CPDU::Peek(sd);
  if (pdu_type == -1)
  {
    // this connection has died 
    Err::Log() << "Error reading lounge socket " << sd << CLog::nl;
    CLB::Inst()->removeLounge(sd, lounge);
  }
  else if (pdu_type != 0)
  { //
    // the whole pdu has arrived - process it
    //
    
    CPDU* pdu = CPDU::Read(sd, pdu_type);
    
    if (pdu)
    {
      pdu->grok(sd, lounge);
      pdu = NULL;
    }
  }
  else
  {
    // Only part of the PDU has arrived, leave it
    // to next iteration
    cout << "Reveived INCOMPLETE PDU" << endl;
  }
}

void CPoller::addLoungeSocket(SOCKET sd)
{
  if (sd != -1)
    FD_SET(sd, &listensds_);
}

void CPoller::removeLoungeSocket(SOCKET sd)
{
  if (sd != -1)
    FD_CLR(sd, &listensds_);
}

void CPoller::newPlayerConnection(SOCKET sd_player)
{
  //
  // New player socket fired 
  //
  struct sockaddr_in cliaddr;
  memset(&cliaddr, 0, sizeof(cliaddr));
  socklen_t clilen = sizeof(cliaddr);
  
  int sockfd = accept(sd_player,
                      (struct sockaddr*)&cliaddr,
                      &clilen);
  
  if (sockfd != INVALID_SOCKET)
  {
    // New player connections are handled
    // completely in separate threads
    CAccepter::Inst()->beginAcceptPlayer(sockfd, &cliaddr);
  }
}

void CPoller::newLoungeConnection(SOCKET sd_lounge)
{

  //
  // New Lounge Server socket fired 
  //
  struct sockaddr_in cliaddr;
  memset(&cliaddr, 0, sizeof(cliaddr));
  socklen_t clilen = sizeof(cliaddr);
  
  int sockfd = accept(sd_lounge,
                      (struct sockaddr*)&cliaddr,
                      &clilen);
  
  if (sockfd != INVALID_SOCKET)
  {
    // New player connections are handled
    // completely in separate threads
    CAccepter::Inst()->beginAcceptLounge(sockfd, &cliaddr);
  }
}
