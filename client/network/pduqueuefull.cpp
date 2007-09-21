/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUQueueFull
  Created  : 05/15/2000

  OVERVIEW : Lounge server sends this PDU to the lounge client
             to notify that an attempt to join a queue failed
             because the queue is full.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduqueuefull.h"
#include "loungedlg/loungedata.h"
#include "loungedlg/resource.h"
#include "loungedlg/loungedlg.h"

BEGIN_NAMESPACE(Network)


void PDUQueueFull::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_

  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
  {
    if (reason_ == QueueFull)
      AfxMessageBox("This waiting list is full.", MB_OK|MB_ICONINFORMATION);
    else if (reason_ == NotEnoughChips)
      AfxMessageBox("You do not have enough chips on your account to play at these limits.", MB_OK|MB_ICONINFORMATION);
    else if (reason_ == TournamentClosed)
      AfxMessageBox("The tournament is closed from new players.", MB_OK|MB_ICONINFORMATION);
    else if (reason_ == AlreadyPlaying)
      AfxMessageBox("You are already playing in the tournament.", MB_OK|MB_ICONINFORMATION);
    else if (reason_ == TournamentSeating)
      AfxMessageBox("Seating is in progress - please try again in a moment.", MB_OK|MB_ICONINFORMATION);
    else if (reason_ == NotValidTournament)
      AfxMessageBox("You need to be on our mailing list in order to play in tournaments.", MB_OK|MB_ICONINFORMATION);

    // Change inQueue status and update buttons
    Lounge::Queue& q = Lounge::GetQueue(queueNumber_);
    if (Lounge::IsValid(q))
    {
      q.inQueue_ = FALSE; // not in queue any more!
    }

    pDlg->refreshButtons();
  }

#endif
}


END_NAMESPACE(Network)

