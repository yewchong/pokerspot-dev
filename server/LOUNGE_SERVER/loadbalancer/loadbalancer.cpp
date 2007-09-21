/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 09/26/2001

  OVERVIEW : This class represents the connection to Load Balancer in
             the Lounge Server.

 ****************************************************************************/

#include "loadbalancer/loadbalancer.h"
#include "loadbalancer/pduloungelogin.h"
#include "loadbalancer/pduloungelogout.h"
#include "loadbalancer/pduplayerloggedin.h"
#include "loadbalancer/pduplayerloggedout.h"
#include "lounge/lounge.h"
#include "server.h"
#include "sys.h"
#include "ssl_funcs.h"
#include <string>
USING_NS_T(std, string);

#define ERROR_LOGGING_ 1

namespace 
{

// XXX MOVE THESE TO SYS.CPP

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
    address.sin_port   = htons(port);
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
        if (Sys_IsSSLEnabled())
          rc = ssl_connect(sd);

        if (rc < 0)
        {
#ifdef ERROR_LOGGING_
          cerr << "ssl_connect fails: " << ::WSAGetLastError() << endl;
#endif
          ssl_close(sd);
          sd = INVALID_SOCKET;
        }
      }
      else
      {
#ifdef ERROR_LOGGING_
        cerr << "connect fails: " << ::WSAGetLastError() << endl;
#endif
        closesocket(sd);
        sd = INVALID_SOCKET;
      }
    }
  }

  return sd;
}

}


CLoadBalancer* CLoadBalancer::pInst_ = NULL;

CLoadBalancer::CLoadBalancer(SOCKET sd)
  :
  sd_(sd)
{
  pInst_ = this;
}

CLoadBalancer::~CLoadBalancer()
{
  closeConnection();
  pInst_ = NULL;
}

CLoadBalancer* CLoadBalancer::ConnectToLoadBalancer(u_long addr, u_short port)
{
  if (pInst_) return pInst_;

  // Do a few attempts to connect to Lounge server
  // before giving up
  for (int i = 0; i < 5; i++)
  {
    SOCKET sd = Sys_MakeConnection(addr, port);
    if (sd != INVALID_SOCKET)
    {
      CLoadBalancer* lb = new CLoadBalancer(sd);
      if (!lb)
      {
        Sys_CloseSocket(sd);
        return NULL;
      }
      else
      {
        if (lb->sendLoungeLogin(Server::Instance()->getPort()) == -1)
        {
          Sys_LogError("Failed to log in to Load Balancer.");
          delete lb;
        }
        else
        {
          Sys_setnonblockio(sd);
          return lb;
        }
      }
    }
    else
    {
      Sys_Sleep(2000);
    }
  }
  
  return NULL;
}

int CLoadBalancer::closeConnection()
{
  if (sd_ != INVALID_SOCKET)
  {
    Sys_CloseSocket(sd_);
    sd_ = INVALID_SOCKET;
  }
  return 0;
}

int CLoadBalancer::sendLoungeLogin(u_int16_t port)
{
  CpduLoungeLogin pdu;
  return pdu.sendLoungeLogin(sd_, port);
}

int CLoadBalancer::sendLoungeUpdate(int numplayers)
{
  //CpduLoungeUpdate pdu;
  //return pdu.sendLoungeLogin(sd_, numPlayers);
  return 0;
}

int CLoadBalancer::sendLoungeLogout()
{
  CpduLoungeLogout pdu;
  return pdu.sendLoungeLogout(sd_);
}
  
int CLoadBalancer::sendPlayerLoggedIn (const char* username)
{
  CpduPlayerLoggedIn pdu;
  return pdu.sendPlayerLoggedIn(sd_, username);
}

int CLoadBalancer::sendPlayerLoggedOut(const char* username)
{
  CpduPlayerLoggedOut pdu;
  return pdu.sendPlayerLoggedOut(sd_, username);
}

void CLoadBalancer::setWaitinglistOwner(int waitinglist, int loungeNumber)
{
  CLounge* thisLounge = CLounge::Inst();

  bool isOwner = (loungeNumber == thisLounge->getNumber());
  CWaitingList* wl = thisLounge->findWaitingList(waitinglist);

  if (wl)
  {
    wl->setOwner(isOwner);
  }
}