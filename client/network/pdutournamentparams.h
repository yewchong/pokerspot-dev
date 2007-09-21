#ifndef __network_pdutournamentparams_h__
#define __network_pdutournamentparams_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUTournamentParams
  Created  : 07/11/2000

  OVERVIEW : Tournament parameters PDU. Table server
             sends this PDU to inform about a change
             in the game parameters.

 ****************************************************************************/

#include "network/network.h"

BEGIN_NAMESPACE(Network)

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

enum BetStructure
{
    BS_Limit    = 1,
    BS_PotLimit = 2,
    BS_NoLimit  = 3
};

// NOTE: these must be identical with
// table server values
enum Server_Game_Type
{
    Game_Type_Holdem            = 0,
    Game_Type_SevenStud         = 1,
    Game_Type_Omaha             = 3,
};



struct PDUTournamentParams : public PDUHeader
{
  // Originated from server
  u_int16_t flag_;
  u_int32_t p1_; 
  u_int32_t p2_; 

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t) +
                                 2 * sizeof(u_int32_t); }

  PDUTournamentParams(const char* buf)
    :
    PDUHeader(buf),
    flag_    (0),
    p1_      (0),
    p2_      (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    flag_ = ntohs(*pint++);

    u_int32_t* plong = (u_int32_t*)pint;
    p1_ = ntohl(*plong++);
    p2_ = ntohl(*plong++);
  }
};


END_NAMESPACE(Network)


#endif