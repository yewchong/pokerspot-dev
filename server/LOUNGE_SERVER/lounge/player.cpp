/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : Class CPlayer
  Created  : 04/16/2000

  OVERVIEW : Player class.

 ****************************************************************************/

#include "lounge/player.h"
#include "pdu/pdu.h"
#include "sys.h"

int CPlayer::send(Cpdu& pdu)
{
  return pdu.sendTo(getSocket());
}


CPingCounter* CPlayer::getPingCounter()
{
  return &pingCounter_;
}
