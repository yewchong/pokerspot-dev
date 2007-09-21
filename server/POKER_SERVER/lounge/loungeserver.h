#ifndef __table_loungeserver_h__
#define __table_loungeserver_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CLoungeServer
  Created  : 04/21/2000

  OVERVIEW : This class maintains connection to the lounge server.

 ****************************************************************************/

#include "compat.h"
#include "psi_win32.h"

class CChips;
class CPlayer;

class CLoungeServer
{
public:
  ~CLoungeServer();

  static CLoungeServer* Inst();
  static CLoungeServer* ConnectToLounge(u_long addr, u_short port);
  int closeConnection();

  int sendTableLogin(u_int16_t gametype,
                     u_int16_t tablenumber,
                     const CChips& tablelo,
                     const CChips& tablehi,
                     u_int16_t tableport);
  int sendTableUpdate(u_int16_t numplayers);
  int sendTableLogout();
  
  int sendPlayerSeated(const char* username);
  int sendPlayerUnseated(CPlayer*);
  int sendPlayerUnseated(const char* username);
  static int SendPlayerUnseated(CPlayer*);
  
  int sendReseatAck(const char* username);
  int sendReseatNack(const char* username);
  
  SOCKET getSocket() const;
  
private:
  
  CLoungeServer(SOCKET);
  
  SOCKET sd_;
  int    max_socket_;
  
  static CLoungeServer* pInst_;
};

inline SOCKET CLoungeServer::getSocket() const
{
    return sd_;
}

inline CLoungeServer* CLoungeServer::Inst()
{
    return pInst_;
}

#endif