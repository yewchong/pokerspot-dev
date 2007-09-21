#ifndef __table_pdudissolveinfo_h__
#define __table_pdudissolveinfo_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveInfo
  Created  : 07/10/2000

  OVERVIEW : The table server sends this PDU to lounge server
             so lounge can reseat the players who are sitting
             at the table.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"
#include "common/chips.h"

struct pdu_dissolveinfo
{
  u_int16_t num_players;
};

// 'num_players_' times:
struct dissolve_playerinfo
{
  char username[PDU_STRINGSIZE];
  CChips::chip_amount_t chips;
};


class CpduDissolveInfo : public Cpdu
{
public:
  CpduDissolveInfo();
  int sendDissolveInfo(SOCKET sd, CTable*);
};


#endif