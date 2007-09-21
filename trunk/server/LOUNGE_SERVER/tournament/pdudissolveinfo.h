#ifndef __lounge_pdudissolveinfo_h__
#define __lounge_pdudissolveinfo_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveInfo
  Created  : 07/10/2000

  OVERVIEW : The table server sends this PDU to lounge server
             so lounge can reseat the players who are sitting
             at the table.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "common/chips.h"

struct pdu_dissolveinfo
{
  u_int16_t num_players;
};

// 'num_players_' times:
struct dissolve_playerinfo
{
  char username_[PDU_STRINGSIZE];
  CChips::chip_amount_t chips_;
};


class CpduDissolveInfo : public Cpdu
{
public:
    CpduDissolveInfo();
    void grok(SOCKET sd, CTable*);

    int getNumPlayers() const;
};

inline int CpduDissolveInfo::getNumPlayers() const
{
    pdu_dissolveinfo* pdu = (pdu_dissolveinfo*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->num_players);
}


#endif