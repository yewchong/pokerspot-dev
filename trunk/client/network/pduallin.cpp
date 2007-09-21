/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAllIn
  Created  : 02/11/2000

  OVERVIEW : All-In PDU; sent by the server when a player
             goes all-in.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduallin.h"
#include "base/gamestate.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/cmdid.h"
#include "tem/community.h"
#include "common/stringoutput.h"

namespace
{
  LPCTSTR g_szAllIn = _T("All In");
  LPCTSTR g_szAllInFmt = _T("%s goes all in with %d chips");
  LPCTSTR g_szAllInNetworkFmt = _T("%s goes all in because of a network timeout");
}


BEGIN_NAMESPACE(Network)

void PDUAllIn::execute(GameState* pS)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pTable = CTableView::Instance();
  Player* pPlayer = pTable ? pTable->getPlayer(slot_) : NULL;

  if (pTable && pPlayer)
  { 
    if (pS)
    { // Let the current state know of the action
      pS->onCommand(MAKEWPARAM(ID_PDU_ALLIN, 0),
                    reinterpret_cast<LPARAM>(this));
    }
    
    Community* pCommunity =
      static_cast<Community*>(pTable->getCommunity());

    CChips chips = pPlayer->getChips();

    pPlayer->setState(Base::AllIn);
    pPlayer->setActionText(g_szAllIn);

    pCommunity->pot_allin(slot_, to_pay_);
    pTable->InvalidateRect(&pCommunity->getTextArea());

    if (pChat)
    {
      CStrOut s;

      //if (to_pay_ > pPlayer->getChips())
      if (pPlayer->getChips() == 0)
      {
        // Ordinary out-of-chips all-in
        // XXX OLD s.Format(g_szAllInFmt, (LPCTSTR)pPlayer->getName(), chips);
        s << (LPCTSTR)pPlayer->getName() << " goes all in with " << chips << " chips";
      }
      else
      {
        // Network all-in
        // XXX OLD s.Format(g_szAllInNetworkFmt, (LPCTSTR)pPlayer->getName());
        s << (LPCTSTR)pPlayer->getName() << " goes all in because of a network timeout";
      }
      pChat->addDealerMessage(s.str(), CChatView::CF_GameCritical);
    }
  }
}


END_NAMESPACE(Network)


