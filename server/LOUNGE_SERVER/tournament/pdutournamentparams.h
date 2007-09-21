#ifndef __lounge_pdutournamentparams_h__
#define __lounge_pdutournamentparams_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentParams
  Created  : 07/08/2000

  OVERVIEW : Send-only tournament parameter pdu.
             Lounge server specifies tournament parameters to
             table server with this pdu.

 ****************************************************************************/

#include "pdu/pdu.h"

class CTable;
class CBaseTournamentQueue;

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

// NOTE: these must be identical with
// table server values
enum Game_Type
{
    Game_Type_Holdem            = 0,
    Game_Type_SevenStud         = 1,
    Game_Type_Omaha             = 3,
};

// NOTE: these must be identical with
// table server values
enum BetStructure
{
    BS_Limit    = 1,
    BS_PotLimit = 2,
    BS_NoLimit  = 3
};


class CpduTournamentParams : public Cpdu
{
public:
    CpduTournamentParams();
    int sendTournamentParams(CTable*   table,
                             u_int16_t flag,
                             u_int32_t p1,
                             u_int32_t p2);
    int sendTournamentParams(CBaseTournamentQueue*,
                             u_int16_t flag,
                             u_int32_t p1,
                             u_int32_t p2);
};


#endif
