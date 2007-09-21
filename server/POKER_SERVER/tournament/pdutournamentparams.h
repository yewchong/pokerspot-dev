#ifndef __table_pdutournamentparams_h__
#define __table_pdutournamentparams_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentParams
  Created  : 07/08/2000

  OVERVIEW : Tournament parameters pdu.
             Lounge server specifies tournament parameters to
             table server with this pdu.
             The table server forwards the pdu to the client
             so as to inform it about the change.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

enum TournamentFlag
{
    TF_Start        = 1,
    TF_GameType     = 2,
    TF_BetStructure = 3,
    TF_Limit        = 4,
    TF_FillSeats    = 5,
    TF_Freeze       = 6,
    TF_Number       = 7,
    TF_Dissolve     = 8,
    TF_End          = 9,
    TF_Pause        = 10,
    TF_Winner       = 11
};

#pragma NETWORK_ALIGN_BEGIN
struct pdu_tournamentparams
{
    u_int16_t flags;
    u_int32_t param1;
    u_int32_t param2;
};
#pragma NETWORK_ALIGN_END

class CpduTournamentParams : public Cpdu
{
public:
    CpduTournamentParams();
    void grok(SOCKET sd, CTable*);

    int sendParams(CPlayer* player,
                   u_int16_t flag,
                   u_int32_t p1,
                   u_int32_t p2);
    int sendParams(CTable*   table,
                   u_int16_t flag,
                   u_int32_t p1,
                   u_int32_t p2);
};


#endif
