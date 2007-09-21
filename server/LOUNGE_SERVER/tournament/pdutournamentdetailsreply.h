#ifndef __lounge_pdutournamentdetailsreply_h__
#define __lounge_pdutournamentdetailsreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentDetailsReply
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament details reply PDU.

 ****************************************************************************/

#include "pdu/pdu.h"

class CTournament;

class CpduTournamentDetailsReply : public Cpdu
{
public:
    CpduTournamentDetailsReply();
    int sendReply(CTournament*, CPlayer*);
};


#endif