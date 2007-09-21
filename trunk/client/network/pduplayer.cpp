/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPlayer
  Created  : 10.11.1999

  OVERVIEW : Player PDUs.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduplayer.h"
#include "network/server.h"
#include "base/player.h"
#include "base/playerstate.h"
#include "base/gamelogic.h"
#include "base/gamestate.h"
#include "base/actionimpl.h"
#include "base/misc.h"
#include "ui/cmdid.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/global.h"

namespace
{
  LPCTSTR g_szPlayerLeft    = _T("%s left the table");
  LPCTSTR g_szPlayerSitsOut = _T("%s sits out");
  LPCTSTR g_szPlayerSitsIn  = _T("%s sits in");
}


namespace Network
{

using Base::GameLogic;

void PDUPlayer::execute(Base::GameState* pState)
{
  CString msg;
  CTableView* pView = CTableView::Instance();
  ASSERT(pView);
  Base::Player* pPlayer = pView->getPlayer(slot_);

  CString username; 
  if (pPlayer)
    username = pPlayer->getName();

  if (pView)
  {
    /* Join case moved to separate PDU! */
    if (state_ & Base::Joined)
    {
      PDU_DEBUG("PDUPlayer with Join status; use PDUPlayerInfo instead!");
    }

    if (state_ & Base::Left)
    {
      if (pPlayer)
      {
        CRect r = pPlayer->getArea();
        // if button or blind leaves have to
        // repaint bigger area
        if (pView->getPlayer(pView->dealer()) == pPlayer ||
            pView->getPlayer(pView->getSmallBlind()) == pPlayer ||
            pView->getPlayer(pView->getBigBlind()) == pPlayer)
        {
          r.top -= (2 * Global::GetCharHeight());
        }
        pView->recalcLayout();
        pView->InvalidateRect(&r);
        msg.Format(g_szPlayerLeft, username);
      }
      pView->removePlayer(slot_);
      pPlayer = NULL;
    }
    if (state_ & Base::SitIn)
    {
      if (pView->getPlayer(slot_) == NULL)
        pView->addPlayer(username, chips_, slot_);

      if (msg.GetLength() > 0 && CChatView::Instance())
        CChatView::Instance()->addDealerMessage(msg, CChatView::CF_GameNormal);

      if (pPlayer && !pPlayer->isSittingIn())
      {
        //msg.Format(g_szPlayerSitsIn, username);
        // Change: there's no information in displaying
        // text "player sits in"
        msg = ""; 
      }
      else
        msg = "";

      pView->setState(slot_, Base::SitIn);
      if (pPlayer)
        pView->InvalidateRect(&pPlayer->getArea());
    }
    if (state_ & Base::SitOut)
    {
      Base::ImplementSitOut(slot_, pPlayer);
      if (pView->isLocalPlayer(slot_) && Base::GameLogic::LeaveTable())
      { // User has clicked the Leave Table button
        // - send logout now
        Base::SetStatusText(pView, _T("Leaving table..."));
        if (Global::TableServer()) 
          Global::TableServer()->sendSetupTableLogout();
      }
    }
    if (state_ & Base::Fold)
    {
      pView->setState(slot_, Base::Fold);
      if (pPlayer)
      {
        pPlayer->empty();
        pView->InvalidateRect(&pPlayer->getArea());
      }
    }

    if (state_ & Base::ChipUpdate)
    {
      pView->updatePlayer(username, chips_, slot_);
    }

    if (state_ & Base::Zombie)
    {
        // Mark player zombie
        if (pPlayer)
        {
            pPlayer->setState(Base::Zombie);
            pView->InvalidateRect(&pPlayer->getArea());
        }
    }
  }

  if (pState)
  { // Tell active state as well
    pState->onCommand(MAKEWPARAM(ID_PDU_PLAYER, 0),
                      reinterpret_cast<LPARAM>(this));
  }

  if (msg.GetLength() > 0 && CChatView::Instance())
    CChatView::Instance()->addDealerMessage(msg, CChatView::CF_GameNormal);
}

} // Network
