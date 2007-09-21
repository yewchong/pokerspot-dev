/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUGameNumber
  Created  : 02/10/2000

  OVERVIEW : Game Number PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdugamenumber.h"
#include "base/gamelogic.h"
#include "base/player.h" // XXX testing
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/actionbar.h"
#include "ui/global.h"

BEGIN_NAMESPACE(Network)

using Base::GameLogic;


void PDUGameNumber::execute(GameState*)
{
  CChatView* pChat = CChatView::Instance();
  CTableView* pTable = CTableView::Instance();

  if (pTable)
    pTable->setGameNumber(gamenumber_);

  if (pChat)
  {
    CString s;
    s.Format("+--- New game #%d ---+", gamenumber_);
    pChat->addDealerMessage(s, CChatView::CF_Fatal);

    // XXX i testing:
/*
    for (int i = 0; i < 10; i++)
    {
        Player* p = pTable->getPlayer(i);
        if (p)
        {
            s.Format("Player %s $%i", p->getName(), p->getChips());
            pChat->addDealerMessage(s, CChatView::CF_Fatal);
        }
    }
*/
  }

  if (!Global::IsObserver())
    CActionBar::UncheckActionButtons();

  GameLogic::NewHand();
}



END_NAMESPACE(Network)
