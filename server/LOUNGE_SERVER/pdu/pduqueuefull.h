#ifndef __lounge_pduqueuefull_h__
#define __lounge_pduqueuefull_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduQueueFull
  Created  : 05/14/2000

  OVERVIEW : Send-only queue full PDU.

 ****************************************************************************/


#include "pdu.h"

struct pdu_queuefull
{
    u_int16_t queueNumber;
    u_int16_t reason;
};


enum QF_Reason
{
    QF_QueueFull         = 0,
    QF_NotEnoughChips    = 1,
    QF_TournamentClosed  = 2,
    QF_AlreadyPlaying    = 3,
    QF_TournamentSeating = 4,
    QF_NotValidForTournament = 5
};


class CpduQueueFull : public Cpdu
{
public:
    CpduQueueFull();
    int sendQueueFull(u_int16_t queuenumber,
                      CPlayer*,                      
                      u_int16_t reason = QF_QueueFull);
};

#endif