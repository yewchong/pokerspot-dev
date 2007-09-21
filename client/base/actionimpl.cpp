/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : actionimpl.cpp
  Created  : 02/13/2000

  OVERVIEW : Implementations for the Ante, Call, Raise and Fold
             actions.

             The functions implement the visualization for
             these actions. There is some logic as well. The
             functions update the players' chips based on the
             action. The real money is of course moved only
             in the server side.

 ****************************************************************************/

#include "stdafx.h"
#include "base/actionimpl.h"
#include "base/player.h"
#include "base/gamelogic.h"
#include "network/network.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/global.h"
#include "ui/soundmanager.h"
#include "tem/community.h"
#include "tem/playerindex.h"
#include "common/chips.h"
#include "common/stringoutput.h"

namespace
{
  LPCTSTR g_szSitOut      = _T("Sitting Out");
  LPCTSTR g_szFold        = _T("Fold");
  LPCTSTR g_szCheck       = _T("Check");
  LPCTSTR g_szCall        = _T("Call");
  LPCTSTR g_szCallFmt     = _T("Call ");
  LPCTSTR g_szBetFmt      = _T("Bet ");
  LPCTSTR g_szRaiseFmt    = _T("Raise ");
  LPCTSTR g_szPostFmt     = _T("Post ");
  LPCTSTR g_szAnteFmt     = _T("Ante ");
  // XXX OLD
  //LPCTSTR g_szCallFmt     = _T("Call %d");
  //LPCTSTR g_szBetFmt      = _T("Bet %d");
  //LPCTSTR g_szRaiseFmt    = _T("Raise %d");
  //LPCTSTR g_szPostFmt     = _T("Post %d");
  //LPCTSTR g_szAnteFmt     = _T("Ante %d");
}

namespace Base
{

using Base::GameLogic;

void ImplementAnte(Player* pPlayer, int slot, const CChips& amount)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Pot* pPot = static_cast<Pot*>(pView->getPlayer(PI_Community));
  if (!pPlayer || !pPot) return;

  CStrOut s;  
  // XXX OLD CString s;

  pPlayer->setState(Base::SitIn);

  if (Global::GetGameType() == GT_SevenStud)
    // XXX OLD s.Format(g_szAnteFmt, amount);
    s << g_szAnteFmt << amount;
  else
    // XXX OLD s.Format(g_szPostFmt, amount);
    s << g_szPostFmt << amount;
  pPlayer->setActionText(s.str());
  s.clear();

  ASSERT(pPlayer->getChips() >= amount);

  pPlayer->setChips(pPlayer->getChips() - amount);
  pPot->pot_ante(pPlayer, slot, amount);
  pView->InvalidateRect(&pPot->getTextArea());

  if (pChat)
  {
    if (Global::GetGameType() == GT_SevenStud)
      // XXX OLD s.Format("%s antes %d", pPlayer->getName(), amount);
      s << pPlayer->getName() << " antes " << amount;
    else
      // XXX OLD s.Format("%s posts %d", pPlayer->getName(), amount);
      s << pPlayer->getName() << " posts " << amount;
    pChat->addDealerMessage(s.str(), CChatView::CF_GameCritical);
  }
}


void ImplementCall(Player* pPlayer, int slot, const CChips& amount)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Pot* pPot = static_cast<Pot*>(pView->getPlayer(PI_Community));
  if (!pPlayer || !pPot) return;

  CStrOut s;

  if (amount != 0)
  {
    // XXX OLD s.Format(g_szCallFmt, amount);
    s << g_szCallFmt << amount;
    if (amount > pPlayer->getChips())
    { // This should never happen since
      // all in is handled as a separate case
      TRACE1("Player %s goes all in\n", pPlayer->getName());
      CChips chips = pPlayer->getChips();
      pPlayer->setChips(0);
    }
    else
    {
      pPlayer->setChips(pPlayer->getChips() - amount);
      pPot->pot_call(pPlayer, slot, amount);
    }
  }
  else
  { // check - no momey movement
    // XXX OLD s = g_szCheck;
    s << g_szCheck;
  }

  pView->InvalidateRect(&pPot->getTextArea());
  pPlayer->setActionText(s.str());
  s.clear();

  if (pChat)
  {
    if (amount == 0)
      // XXX OLD s.Format("%s checks", (LPCTSTR)pPlayer->getName());
      s << pPlayer->getName() << " checks";
    else
      // XXX OLD s.Format("%s calls %d", (LPCTSTR)pPlayer->getName(), amount);
      s << pPlayer->getName() << " calls " << amount;
    pChat->addDealerMessage(s.str(), CChatView::CF_GameCritical);
  }
}


void ImplementRaise(Player* pPlayer, int slot, const CChips& current_bet, const CChips& raise)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Pot* pPot = static_cast<Pot*>(pView->getPlayer(PI_Community));
  if (!pPlayer || !pPot) return;

  CStrOut s;

  if (current_bet == 0)
    // XXX OLD s.Format(g_szBetFmt, raise); 
    s << g_szBetFmt << raise; // if bet was 0, its a "bet"
  else
    // XXX OLD s.Format(g_szRaiseFmt, raise);
    s << g_szRaiseFmt << raise;
  pPlayer->setActionText(s.str());
  s.clear();

  BOOL wasComplete = pPot->pot_isComplete();

  CChips to_pay = current_bet + raise;
  // If this fails, then the server should have
  // sent "all-in" for this player.
  PDU_ASSERT(pPlayer->getChips() >= to_pay);
  pPlayer->setChips(pPlayer->getChips() - to_pay);
  pPot->pot_raise(pPlayer, slot, current_bet, raise);

  pView->InvalidateRect(&pPot->getTextArea());

  BOOL isComplete = pPot->pot_isComplete();

  if (pChat)
  {
    if (current_bet == 0)
    {
      if (Global::GetGameType() == GT_SevenStud &&
          GameLogic::IsBringIn())
      {
        // XXX OLD s.Format("%s posts the bring-in",
        //         (LPCTSTR)pPlayer->getName());
        s << pPlayer->getName() << " posts the bring-in";
      }
      else
      {
        // XXX OLD s.Format("%s bets %d", (LPCTSTR)pPlayer->getName(), raise);
        s << (LPCTSTR)pPlayer->getName() << " bets " << raise;
      }
    }
    else
    {
      // XXX OLD s.Format("%s raises %d", (LPCTSTR)pPlayer->getName(), raise);
      s << (LPCTSTR)pPlayer->getName() << " raises " << raise;
    }

    if (!wasComplete && isComplete)
      // XXX OLD s += " and completes the bet";
      s << " and completes the bet";

    pChat->addDealerMessage(s.str(), CChatView::CF_GameCritical);
  }
}


void ImplementFold(Player* pPlayer, int slot)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Pot* pPot = static_cast<Pot*>(pView->getPlayer(PI_Community));
  if (!pPlayer || !pPot) return;

  CString s;

  pPot->pot_fold(pPlayer, slot);

  // the player must be invalidated before cards are
  // removed and area becomes smaller!
  pView->InvalidateRect(pPlayer->getArea());

  pPlayer->setState(Base::Fold);
  pPlayer->setActionText(g_szFold);
  pPlayer->setFolded(TRUE);
  pPlayer->empty();

  // Move player's bet chips to pot, if any
  CChips betChips = pPlayer->getBetChips();
  pPot->setDrawChips(pPot->getDrawChips() + betChips);
  pPlayer->setBetChips(0);

  if (pChat)
  {
    s.Format("%s folds", (LPCTSTR)pPlayer->getName());
    pChat->addDealerMessage(s, CChatView::CF_GameCritical);
  }

  SndMan::Instance()->playSound(SndMan::SM_Fold);
}


void ImplementSitOut(USHORT slot, Player* pPlayer)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Player* pPot = pView->getPlayer(PI_Community);
  if (!pPlayer || !pPot) return;

  //int oldState = pPlayer->getState();
  pView->setState(slot, Base::SitOut);
  pPlayer->setActionText(g_szSitOut);

  if (pChat != NULL) // && oldState != Base::SitOut      
  {
    CString s;
    s.Format("%s sits out", pPlayer->getName());
    pChat->addDealerMessage(s, CChatView::CF_GameNormal);
  }
}

} // namespace