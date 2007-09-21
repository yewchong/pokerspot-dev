/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CLoungeServer
  Created  : 04/21/2000

  OVERVIEW : This class manages connection to the lounge server.

 ****************************************************************************/

#include "lounge/loungeserver.h"
#include "lounge/pdutablelogin.h"
#include "lounge/pdutableupdate.h"
#include "lounge/pdutablelogout.h"
#include "includes.h"
#include "sys.h"
#include "pduhandler/pduhandler.h"
#include "tournament/pduplayerseated.h"
#include "tournament/pduplayerunseated.h"
#include "tournament/pdureseatack.h"
#include "tournament/tournament.h"
#include "player/player.h"
#include "ssl_funcs.h"
#include <string>

/*
 * Module helper functions
 */
static SOCKET MakeConnection(UINT ipaddr, short port);
static BOOL MakeSockAddr(const char* host, short port, struct sockaddr* pResult);


CLoungeServer* CLoungeServer::pInst_ = NULL;

CLoungeServer::CLoungeServer(SOCKET sd)
    :
    sd_(sd),
    max_socket_(sd + 1)
{
    pInst_ = this;
}

CLoungeServer::~CLoungeServer()
{
    pInst_ = NULL;
}

CLoungeServer* CLoungeServer::ConnectToLounge(u_long addr, u_short port)
{
    // do a few attempts to connect to Lounge server
    // before giving up
    for (int i = 0; i < 5; i++)
    {
        SOCKET sd = MakeConnection(addr, port);
        if (sd != INVALID_SOCKET)
        {
            return new CLoungeServer(sd);
        }
        else
            Sys_Sleep(2000);
    }

    return NULL;
}

int CLoungeServer::closeConnection()
{
    if (sd_ != INVALID_SOCKET)
    {
        Sys_close(sd_);
        sd_ = INVALID_SOCKET;
    }
    return 0;
}

int CLoungeServer::sendTableLogin(u_int16_t gametype,
                                  u_int16_t tablenumber,
                                  const CChips& tablelo,
                                  const CChips& tablehi,
                                  u_int16_t tableport)
{
    CpduTableLogin pdu;
    return pdu.sendTableLogin(sd_,
                              gametype,
                              tablenumber,
                              tablelo,
                              tablehi,
                              tableport);
}

int CLoungeServer::sendTableUpdate(u_int16_t numplayers)
{
    CpduTableUpdate pdu;
    return pdu.sendTableUpdate(sd_, numplayers);
}

int CLoungeServer::sendTableLogout()
{
    CpduTableLogout pdu;
    return pdu.sendTableLogout(sd_);
}


/*
 * Helper function implementation
 */
BOOL MakeSockAddr(const char* host, short port, struct sockaddr* pResult)
{
  ASSERT(host != NULL && *host != '\0');

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
    address.sin_port   = port;
    memcpy(&address.sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
    memcpy(pResult, &address, sizeof(address));
  }

  return pHostEnt != NULL;
}  

SOCKET MakeConnection(UINT ipaddr, short port)
{
  SOCKET sd = INVALID_SOCKET;
  struct sockaddr sockAddr;
  
  struct in_addr inaddr;
  inaddr.s_addr = ipaddr;
  char* p = inet_ntoa(inaddr);
  if (p == NULL)
    return INVALID_SOCKET;
  string host = p;

  if (ipaddr == 0 || host == "127.0.0.1")
  { // special case to ease local testing
    host = "localhost";
  }

  if (MakeSockAddr(host.c_str(), port, &sockAddr))
  {    
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd != INVALID_SOCKET)
    {
      int rc = connect(sd, &sockAddr, sizeof(sockAddr));
      if (rc >= 0)
      { 
          if (Sys_IsSSLEnabled())
              rc = ssl_connect(sd);
      }
      else
      {
        printf("Connect fails: %i\n", Sys_GetLastError());
      }
      
      if (rc < 0)
      {
        Sys_LogError("connect fails");
        closesocket(sd);
        sd = INVALID_SOCKET;
      }
    }
  }

  return sd;
}  

int CLoungeServer::sendPlayerSeated(const char* username)
{
    CpduPlayerSeated pdu;
    return pdu.sendPlayerSeated(sd_, username);
}


int CLoungeServer::sendPlayerUnseated(CPlayer* player)
{
    if (player->isFreeSeat())
        return 0;

    if (CTournament::Inst()->isTournament())
    {
        char buf[100];
        sprintf(buf, "Player %s finished", player->getUsername());
        Sys_LogTournament(buf);

        CTournament::Inst()->playerFinished(player);
    }

    return sendPlayerUnseated(player->getUsername());
}


int CLoungeServer::sendPlayerUnseated(const char* username)
{
    CpduPlayerUnseated pdu;
    return pdu.sendPlayerUnseated(sd_, username);
}

int CLoungeServer::sendReseatAck(const char* username)
{
    CpduReseatAck pdu;
    return pdu.sendAck(sd_, username);
}


int CLoungeServer::sendReseatNack(const char* username)
{
    CpduReseatAck pdu;
    return pdu.sendNack(sd_, username);
}

int CLoungeServer::SendPlayerUnseated(CPlayer* p)
{
    if (Inst())
    {
        return Inst()->sendPlayerUnseated(p);
    }
    else
    {
        return 0;
    }
}