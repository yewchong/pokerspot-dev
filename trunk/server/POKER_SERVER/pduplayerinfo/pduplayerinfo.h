#ifndef __table_pduplayerinfo_h__
#define __table_pduplayerinfo_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerInfo
  Created  : 1/1/2001

  OVERVIEW : This class is used by the table server to send
             - current state of players to a new player
             - state of new player to players currently at the table

 ****************************************************************************/

#include "pduhandler/pduhandler.h"
#include "common/chips.h"

class CPlayer;
class CTable;

typedef struct pdu_playerinfo pdu_playerinfo_t;
typedef struct player_entry player_entry_t;

struct pdu_playerinfo
{
  u_int16_t num_players;
};

#pragma NETWORK_ALIGN_BEGIN
struct player_entry
{
  u_int16_t slot;
  CChips::chip_amount_t chips;
  u_int16_t state;
  char username[PDUSTRINGSIZE];
  char city[PDUSTRINGSIZE];
};
#pragma NETWORK_ALIGN_END


class CpduPlayerInfo : public Cpdu
{
public:
	CpduPlayerInfo(CTable*);
	int sendPlayerInfo(CPlayer*);
  int sendObserverInfo(CPlayer*);

private:
	CTable* table_;
};

#endif