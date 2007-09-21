/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class GameLogic
  Created  : 30.10.1999

  OVERVIEW : Base class for game logic.

             User actions and network messages from the
             server schedule state objects to be executed.
             Class GameLogic acts as a manager of the
             scheduled states. Each state is executes until
             nextState() is called, which destroys the
             current state and takes the next state from
             the queue.

 ****************************************************************************/

#include "stdafx.h"
#include "base/gamelogic.h"
#include "base/gamestate.h"
#include "network/network.h"
#include "network/server.h"
#include "ui/global.h"
#include "ui/chatview.h"
#include "ui/cmdid.h"
#include "ui/registry.h"
#include "ui/actionbar.h"
#include "common/chips.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  Base::GameLogic g_GameLogic;
}


namespace Base
{

GameLogic& GameLogic::Instance()
{
  return g_GameLogic;
}


GameLogic::GameLogic()
  :
  pCurrentState_(0),
  callInTurn_   (FALSE),
  raiseInTurn_  (FALSE),
  foldInTurn_   (FALSE),
  sitOut_       (FALSE),
  autoPostBlind_(FALSE),
  hideHand_     (FALSE),
  leaveTable_   (FALSE),
  takeFocusOnRequest_(FALSE),
  bringIn_      (0)
{}


GameLogic::~GameLogic()
{
  destroyStates();
}


BOOL GameLogic::tick(DWORD dt)
{
  BOOL rc = FALSE;

  if (!pCurrentState_)
    nextState();

  if (pCurrentState_)
    rc = pCurrentState_->tick(dt);

  return rc;
}


BOOL GameLogic::draw(CDC* pDC)
{
  BOOL rc = FALSE;

  if (pCurrentState_)
    rc = pCurrentState_->draw(pDC);

  return rc;
}


//
// Handle unhandled game PDUs here.
//
BOOL GameLogic::onCommand(WPARAM wParam, LPARAM lParam)
{
  BOOL rc = FALSE;

  int cmd = LOWORD(wParam);

  if (cmd == ID_PDURECEIVED && (lParam != 0))
  { //
    // Received a PDU - handle it here
    //
    rc = handlePDU(lParam);
  }

  if (!rc && pCurrentState_)
  { //
    // An ordinary command - let the current state handle it
    //
    rc = pCurrentState_->onCommand(wParam, lParam);
  }

  return rc; 
}


BOOL GameLogic::onKey(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  BOOL rc = FALSE;

  if (pCurrentState_)
    rc = pCurrentState_->onKey(nChar, nRepCnt, nFlags);

  return rc; 
}


void GameLogic::SendCallActionPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlayActionCallPDU();
#endif
}


void GameLogic::SendRaiseActionPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlayActionRaisePDU();
#endif
}


void GameLogic::SendSpreadRaiseActionPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
  {
    CActionBar* bar = CActionBar::Instance();    
    Global::TableServer()->sendPlayActionSpreadRaisePDU(bar->getRaise());
  }
#endif
}


void GameLogic::SendFoldActionPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlayActionFoldPDU();
#endif
}


void GameLogic::SendSitOutPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlaySitOutPDU();
#endif
}


void GameLogic::SendSitInPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlaySitInPDU();
#endif
}


void GameLogic::SendPostBlindPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlayPostBlindPDU();
#endif
}


void GameLogic::SendRefuseBlindPDU()
{
#ifdef PSPOT_TABLE_MODULE_
  if (Global::TableServer())
    Global::TableServer()->sendPlayRefuseBlindPDU();
#endif
}


void GameLogic::SendShowCardsPDU()
{
  if (Global::TableServer())
    Global::TableServer()->sendPlayShowCardsPDU();
}


void GameLogic::SendHideCardsPDU()
{
  if (Global::TableServer())
    Global::TableServer()->sendPlayHideCardsPDU();
}


BOOL GameLogic::showMotd()
{
  // return scheduleSate(new StateMotd());
  return FALSE;
}


BOOL GameLogic::floorMap()
{
  // return scheduleState(new StateFloorMap());
  return FALSE;
}


BOOL GameLogic::joinTable()
{
  // return scheduleState(new StateJoinTable());
  return FALSE;
}


BOOL GameLogic::leaveTable()
{
  //return scheduleState(new StateLeaveTable());
  return FALSE;
}


BOOL GameLogic::scheduleState(GameState* pGS)
{
  states_.push_back(pGS);
  return TRUE;
}


BOOL GameLogic::setNextState(GameState* pGS)
{
  setState(pGS);
  return TRUE;
}


BOOL GameLogic::resetStates()
{
  destroyStates();
  return TRUE;
}


BOOL GameLogic::nextState()
{
  BOOL rc = FALSE;

  States::iterator i = states_.begin(),
                   e = states_.end();
  if (i != e)
  {
    GameState* pGS = (*i);
    states_.erase(i);
    setState(pGS);
    rc = TRUE;
  }

  return rc;  
}


void GameLogic::destroyStates()
{
  for (States::iterator i = states_.begin(),
                        e = states_.end();
       i != e;
       ++i)
  {
    delete (*i);
  }

  states_.erase(states_.begin(), states_.end());

  delete pCurrentState_;
  pCurrentState_ = 0;
}


void GameLogic::setState(GameState* pGS)
{
  if (pCurrentState_)
    delete pCurrentState_;
  pCurrentState_ = pGS;
}


//
// Handle PDU by calling its execute() mfunction.
// PDUs know how to handle themselves.
//
BOOL GameLogic::handlePDU(LPARAM lParam)
{
  ASSERT(lParam != 0);
  if (lParam != 0)
  {
    using namespace Network;
    PDUHeader* pPDU = reinterpret_cast<PDUHeader*>(lParam);

    // Let the PDU implement its logic.
    pPDU->execute(pCurrentState_);

    // All PDU's are deallocated here after
    // they have done their job.
    delete pPDU;
  }

  return TRUE;
}


void GameLogic::HideHand(BOOL b)
{
  if (b != Instance().hideHand_)
  {
    Instance().hideHand_ = b;

    // Inform the server immediately
    if (Global::TableServer())
    {
      if (b)
        Global::TableServer()->sendPlayHideCardsPDU();
      else
        Global::TableServer()->sendPlayShowCardsPDU();
    }
  }
}

void GameLogic::SitOut(BOOL b)
{
  if (b != Instance().sitOut_)
  {
    Instance().sitOut_ = b;

    // Inform the server immediately
    if (Global::TableServer())
    {
      if (b)
        Global::TableServer()->sendPlaySitOutPDU();
      else
        Global::TableServer()->sendPlaySitInPDU();
    }
  }
}


void GameLogic::NewHand()
{
  Instance().bringIn_ = 0;
}

void GameLogic::CardsDealt()
{
  Instance().bringIn_++;
}

BOOL GameLogic::IsBringIn()
{
  BOOL rc = FALSE;

  // First bet after cards have been dealt
  // is the bring in
  if (Instance().bringIn_ == 1)
    rc = TRUE;

  Instance().bringIn_++;

  return rc;
}

BOOL GameLogic::GetIsBringIn()
{
  // First bet after cards have been dealt
  // was the bring in
  return (Instance().bringIn_ == 1);
}


} // Base


