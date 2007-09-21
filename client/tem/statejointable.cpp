/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateJoin
  Created  : 9.11.1999

  OVERVIEW : Implements joining to a table for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/statejointable.h"
#include "base/gamelogic.h"
#include "ui/tableview.h"


using namespace Network;


//
// STATE: StateJoinTable
//        Joining a table. The server enumerates
//        the players in the table to this (new)
//        client.
//
// Entry: Successfull table login.
//
// PDUs:  PDU_Player
//
// Exit:  StateHandStart on all players enumerated.
//

StateJoinTable::StateJoinTable()
{
}


BOOL StateJoinTable::tick(DWORD)
{ // No idle time processing
  return FALSE;
}


BOOL StateJoinTable::draw(CDC*)
{ // Nothing to render
  return FALSE;
}


/*
BOOL StateJoinTable::onPDU(PDUHeader* pPDU)
{
  BOOL rc = FALSE;

  if (IsPDU(pPDU, PDU_Player))
  {
    PDUPlayer* pP = static_cast<PDUPlayer*>(pPDU);
    onPlayer(pP);
    bool isLast = (pP->numplayers_ == pP->player_);
    delete pPDU;

    if (isLast)
    { // Enumeration is complete
      //GameLogic::setNextState(new StateHandStart());
    }
    rc = TRUE;
  }

  if (!rc)
    rc = GameState::onPDU(pPDU);

  return rc;
}

BOOL StateJoinTable::onPlayer(PDUPlayer* pP)
{
  bool isThisPlayer = false;
    //(pP->username_ == GameLogic::Username());

  // XXX Record THIS player!

  //CTableView::Instance()->addPlayer(pP->username_,
//                                    pP->chips_);

  return TRUE;
}

*/