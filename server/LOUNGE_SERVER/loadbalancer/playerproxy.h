#ifndef __lounge_playerproxy_h__
#define __lounge_playerproxy_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : Class CPlayerProxy
  Created  : 10/03/2001

  OVERVIEW : This CPlayer subclass is used as a proxy to represent
             a player who is logged in to another Lounge Server.

 ****************************************************************************/


#include "lounge/player.h"


class CPlayerProxy : public CPlayer
{
public:
  CPlayerProxy(const string& username)
    : 
    CPlayer(username)
  {}

  // CPlayer overrides
  int send(Cpdu&);
  CPingCounter* getPingCounter();
};

typedef hash_map<string, CPlayerProxy, hash_string> RemotePlayers;

#endif
