#ifndef __lounge_server_h__
#define __lounge_server_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Server
  Created  : 04/15/2000

  OVERVIEW : Lounge server.

 ****************************************************************************/

#include "compat.h"
#include "ssl_funcs.h"


class Server
{
public:
  Server();
  ~Server();
  
  static Server* Instance();
  static Server* Inst();
  
  int  initialize(int argc, char* argv[]);
  int  readConfig();
  int  processConfig(u_short tableport);
  int  mainLoop();
  void shutdown();
  
  void controlBreak();
  void checkShutdown();
  void quit(bool);

  u_short getPort() const;
  
private:
  SOCKET  sd_player_, sd_table_;
  u_short port_;
  bool    quit_;
  
  static Server* pInstance_;
};


inline Server* Server::Instance()
{
  return pInstance_; 
}

inline Server* Server::Inst()
{
  return pInstance_; 
}

inline void Server::quit(bool b)
{
  quit_ = b;
}

inline u_short Server::getPort() const
{
  return port_;
}

#endif
