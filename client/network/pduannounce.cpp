/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : struct PDUAnnounce
  Created  : 16.12.1999

  OVERVIEW : Announce PDUs.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduannounce.h"
#include "network/server.h"
#include "base/gamelogic.h"
#include "base/gamestate.h"
#include "base/player.h"
#include "base/misc.h"
#include "ui/motddlg.h"
#include "ui/chatview.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/soundmanager.h"
#include "ui/cmdid.h"

namespace
{
  LPCTSTR g_szAnnounce = _T("Announcement");
}


BEGIN_NAMESPACE(Network)

USING_NS_T(Base, GameLogic);

// Special message-only announcement
#define ANNOUNCE_FLAG_MSG 0xfffe

void PDUAnnounce::execute(GameState*)
{
  CTableView* pTable = CTableView::Instance();

  bool msgOnly = (winner_ == ANNOUNCE_FLAG_MSG);

  if (!msgOnly && pTable)
  {
    // Tell the current state to quit animations if any
    GameState* state = GameLogic::Instance().currentState();
    if (state)
      state->onCommand(MAKEWPARAM(ID_STOPANIMATION, 0), 0);
    pTable->endBettingRound(true);
  }

  CChatView* pChat = CChatView::Instance();
  if (pChat && pTable)
  {
    CString name("unknown");
    Base::Player* p = pTable->getPlayer(winner_);
    if (p)
    {
      name = p->getName();
      pTable->showWinner(winner_, 3000); // animate 3 secs
    }

    // Replace newlines with spaces
    CString s(message_);    
    for (int i = 0; i < s.GetLength(); i++)
    {
      if (s[i] == '\n' || s[i] == '\r')
        s.SetAt(i, ' ');
    }

    if (msgOnly)
      pChat->addDealerMessage(s, CChatView::CF_GameNormal);
    else
      pChat->addDealerMessage(s, CChatView::CF_Fatal);

    if (!msgOnly)
    {
      CChips chipsInPot = pTable->resetPot();

      if (pTable->isLocalPlayer(winner_))
      {
        if (chipsInPot > 100)
          SndMan::Instance()->playSound(SndMan::SM_RakeInLarge);
        else
          SndMan::Instance()->playSound(SndMan::SM_RakeInSmall);
      }
      else
      {
        SndMan::Instance()->playSound(SndMan::SM_Show);
      }
    }

#ifdef SHOW_ANNOUNCEMENT_RECT_
    // The announcement rectangle is not currently used.
    pTable->announce(message_);
#endif
  }  

  if (!msgOnly && GameLogic::LeaveTable())
  { // User has clicked the Leave Table button
    // - send logout now
    if (pTable)
      Base::SetStatusText(pTable, _T("Leaving table..."));
    if (Global::TableServer()) 
      Global::TableServer()->sendSetupTableLogout();
  }
}


END_NAMESPACE(Network)