/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : pdugamenames.h
  Created  : 05/08/2000

  OVERVIEW : Lounge Game Names PDU.

 ****************************************************************************/

#include "stdafx.h"
#ifndef PSPOT_TABLE_MODULE_
  #include "loungedlg/floormap.h"
#endif
#include "network/pduloungegamenames.h"

BEGIN_NAMESPACE(Network)

void PDUGameNames::execute(GameState*)
{
  CFloorMap* pFM = CFloorMap::Instance();

  if (pFM)
  {
    GameName* pG = pNames_;
    for (int i = 0; i < numGames_; i++, pG++)
    {
      pFM->addGame(pG->gameType_,
                   pG->gameName_);
    }
  }
}


END_NAMESPACE(Network)

