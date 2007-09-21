#ifndef __table_pduplayerfinished_h__
#define __table_pduplayerfinished_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerFinished
  Created  : 08/22/2000

  OVERVIEW : This pdu is used to notify clients that a player was eliminated
             from tournament.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"

class CTable;
class CPlayer;

class CpduPlayerFinished : public Cpdu
{
public:
    CpduPlayerFinished();
    int sendPlayerFinished(CTable*   table,
                           CPlayer*  player,
                           u_int16_t tableNum,
                           u_int16_t pos,
                           u_int16_t players);
};


#endif