/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StatePromptPostBlind
  Created  : 18.11.1999

  OVERVIEW : Prompt the user whether to post the blind.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/statepromptpostblind.h"
#include "tem/playerindex.h"
#include "network/pduplayer.h"
#include "network/pduactionecho.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "base/misc.h"
#include "ui/tableview.h"
#include "ui/mainwnd.h"
#include "ui/actionbar.h"
#include "ui/cmdid.h"
#include "ui/soundmanager.h"
#include "ui/global.h"
#include "common/stringoutput.h"

using Base::GameLogic;
using namespace Network;

namespace
{
  LPCTSTR g_szPostPromptFmt = _T("Post ");
  LPCTSTR g_szPostFmt   = _T("To Post ");
  LPCTSTR g_szAntePromptFmt = _T("Ante ");
  LPCTSTR g_szAnteFmt   = _T("To Ante ");
  LPCTSTR g_szSitOut    = _T("Sit Out");
  LPCTSTR g_szWaitingOthers = _T("Waiting for others to ante");

  LPCTSTR g_szBlindStatusFmt = _T("Post %d or Sit Out");
  LPCTSTR g_szWaitingFmt     = _T("Waiting for %s to act");

  StatePromptPostBlind* g_pState = 0;

  VOID CALLBACK TimerProc(HWND, UINT msg, UINT eventId, DWORD dwTime)
  {
    if (g_pState)
      g_pState->onTimer(msg, eventId, dwTime);
  }
}

extern LPCTSTR g_szOutOfTimers;

StatePromptPostBlind::StatePromptPostBlind(int player,
                                           const CChips& value,
                                           unsigned int msecs)
  :
  player_        (player),
  value_         (value),
  ticks_         (0),
  waitActionTime_(msecs),
  timeLeft_      (0),
  timer_         (0),
  pView_         (CTableView::Instance()),
  doChipsAnimate_(-1)
{}


StatePromptPostBlind::~StatePromptPostBlind()
{
  chipsAnim_.stopAnimate();
  endPrompt();
}


void StatePromptPostBlind::start()
{
  BOOL isSevenStud = (Global::GetGameType() == GT_SevenStud);

  bool replied = false;
  if (pView_->isLocalPlayer(player_))
  { // Blind request is for us - reply
    if (GameLogic::SitOut() || GameLogic::LeaveTable())
    { //
      // User wants to sit out - send Refuse Blind PDU
      //
      replied = true;
      GameLogic::SendRefuseBlindPDU();
      endPrompt();

      if (isSevenStud)
      {
        CMainWnd::SetStatusText(g_szWaitingOthers);
        Player* p = pView_->getPlayer(player_);
        if (p)
        {
          p->setActionText("Sit Out");
        }
      }
    }
    else if (GameLogic::AutoPostBlind())
    { //
      // User has auto post of blind on - send Post Blind PDU
      //
      replied = true;
      GameLogic::SendPostBlindPDU();
      endPrompt();
      
      if (isSevenStud)
      { // Display action immediately
        Player* p = pView_->getPlayer(player_);
        if (p)
        {
          CStrOut s;
          s << "Ante " << value_;
          // XXX OLD CString s;
          // s.Format("Ante %d", value_);
          p->setActionText(s.str());
        }
        CMainWnd::SetStatusText(g_szWaitingOthers);
        doChipsAnimate_ = player_;
        chipsAnim_.startAnimate(doChipsAnimate_,
                                PI_Community,
                                value_,
                                SndMan::SM_Call);
      }
    }
  }

  g_pState = this;

  if (!replied)
  {
    if (isSevenStud && Global::IsObserver())
    {
        // do nothing
    }
    else
    {
        turnPtr_.setTurn(player_, waitActionTime_);
        turnPtr_.tick(0);

        if ((timer_ = ::SetTimer(NULL, 0, 500, TimerProc)) == 0)
            pView_->MessageBox(g_szOutOfTimers, NULL, MB_OK|MB_ICONEXCLAMATION);

        // do first timer right away
        onTimer(WM_TIMER, timer_, 0);
    }
  }
}


BOOL StatePromptPostBlind::tick(DWORD dt)
{
  if (doChipsAnimate_ != -1)
  {
    doChipsAnimate_ = -1;
    dt = 1;
  }

  return chipsAnim_.stepAnimate(dt);
}


BOOL StatePromptPostBlind::draw(CDC* pDC)
{
  turnPtr_.draw(pDC);
  return TRUE;
}


BOOL StatePromptPostBlind::onCommand(WPARAM wParam, LPARAM lParam)
{
  BOOL isSevenStud = (Global::GetGameType() == GT_SevenStud);
  BOOL rc = FALSE;
  UINT cmd = LOWORD(wParam);
  BOOL quit = FALSE;
  
  switch (cmd)
  {
  case ID_BTN_1:
    GameLogic::SendRefuseBlindPDU();
    endPrompt();

    if (isSevenStud)
    {
      Player* p = pView_->getPlayer(player_);
      if (p)
      {
        p->setActionText("Sit Out");
      }
      CMainWnd::SetStatusText(g_szWaitingOthers);
    }

    rc = TRUE;
    break;

  case ID_BTN_2:
    GameLogic::SendPostBlindPDU();
    endPrompt();

    if (isSevenStud)
    { // Display action immediately
      Player* p = pView_->getPlayer(player_);
      if (p)
      {
        CStrOut s;
        s << "Ante " << value_;
        // XXX OLD CString s;
        // s.Format("Ante %d", value_);
        p->setActionText(s.str());
      }
      doChipsAnimate_ = player_;
      chipsAnim_.startAnimate(doChipsAnimate_,
                              PI_Community,
                              value_,
                              SndMan::SM_Call);
      CMainWnd::SetStatusText(g_szWaitingOthers);
    }

    rc = TRUE;
    break;

  case ID_PDU_ACTIONECHO:
  {
    //
    // The PDU implements the action -
    // end prompting
    //
    PDUActionEcho* pPDU = reinterpret_cast<PDUActionEcho*>(lParam);
    if (!isSevenStud)
    { 
      endPrompt();
    }

    if (pPDU)
    {
      // if money moving action, start chips animation
      if (pPDU->action_ == PDUAction::Ante &&
          pPDU->amount_ != 0)
      {
        if (isSevenStud && pView_->isLocalPlayer(pPDU->slot_))
          ; // 7-stud does animation on local action
        else
        { // 7-stud: Local player does animation on local action,
          // others here when server echoes the action
          doChipsAnimate_ = pPDU->slot_;
          chipsAnim_.startAnimate(doChipsAnimate_,
                                  PI_Community,
                                  pPDU->amount_,
                                  SndMan::SM_Call);
        }
      }
    }
    
    rc = TRUE;
    break;
  }

  case ID_PDU_PLAYER:
  {
    PDUPlayer* p = reinterpret_cast<PDUPlayer*>(lParam);
    if (((p->state_ & Base::SitOut) ||
         (p->state_ & Base::Left)) &&
        p->slot_ == player_)
    { // The player we're prompting sits out or leaves - 
      // quit this state
      rc = TRUE; 
      if (!isSevenStud)
        quit = TRUE;
    }
  }

  case ID_ISPROMPTBLINDPOST:
    rc = TRUE;
    break;

  case ID_STOPANIMATION:
  {
    chipsAnim_.stopAnimate();
    rc = TRUE;
    break;
  }

  default:
    break;
  }

  if (quit)
  {   
    GameLogic::Instance().setNextState(0);
  }

  return rc;
}


BOOL StatePromptPostBlind::onKey(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  return FALSE;
}


BOOL StatePromptPostBlind::onTimer(UINT msg, UINT eventId, DWORD dwTime)
{
  bool playSound = false;
  ASSERT_VALID(pView_);
  bool isForMe = pView_->isLocalPlayer(player_) && !Global::IsObserver();

  Player* pP = pView_->getPlayer(player_);

  if (ticks_ == 0)
  { // first call
    CStrOut s;
    if (pP)
    {
      if (Global::GetGameType() == GT_SevenStud)
        s << g_szAnteFmt << value_;
      else
        s << g_szPostFmt << value_;
      pP->setActionText(s.str());
      s.clear();
    }

    if (isForMe)
    { // The request is for me - enable UI
      CActionBar* pActionBar = CActionBar::Instance();

      bool isTournament = Global::IsTournament();
      if (pActionBar && !isTournament)
      {
        if (Global::GetGameType() == GT_SevenStud)
          s << g_szAntePromptFmt << value_;
        else
          s << g_szPostPromptFmt << value_;

        pActionBar->setButtonText(0, g_szSitOut);
        pActionBar->setButtonText(1, s.str());
        pActionBar->enableButtons(1 | 2);
      }

      if (!isTournament)
      {
        s.clear();
        s << "Post " << value_ << " or Sit Out";
        CMainWnd::SetStatusText(s.str());
        playSound = true;
      }
    }
    else
    {
      if (pP)
      {
        s.clear();
        s << "Waiting for " << pP->getName() << " to act";
        CMainWnd::SetStatusText(s.str());
      }
    }
  }

  ticks_ += 500;

  if (isForMe)
    CActionBar::Instance()->animate(500);
  turnPtr_.tick(ticks_);

  if (ticks_ > waitActionTime_)
  {
    if (isForMe)
    {
      // If the player does not act in time,
      // the default action is to sit out
      GameLogic::SendRefuseBlindPDU();
    }

    endPrompt();

    if (isForMe)
    {
      // Player times out and won't reply -
      // turn sit-out flag automatically on
      CActionBar::CheckSitOut(TRUE);
    }
  }
  else
  {
    if (isForMe && playSound)
      SndMan::Instance()->playSound(SndMan::SM_YourTurn);

    if (isForMe && waitActionTime_ - ticks_ == 4500)
      SndMan::Instance()->playSound(SndMan::SM_WakeUp);
    else if (isForMe && waitActionTime_ - ticks_ == 3500)
      SndMan::Instance()->playSound(SndMan::SM_WakeUp);
    else if (isForMe && waitActionTime_ - ticks_ == 2500)
      SndMan::Instance()->playSound(SndMan::SM_WakeUp);
    else if (isForMe && waitActionTime_ - ticks_ == 1500)
      SndMan::Instance()->playSound(SndMan::SM_WakeUp);
    else if (isForMe && waitActionTime_ - ticks_ == 500)
      SndMan::Instance()->playSound(SndMan::SM_WakeUp);
  }

  return TRUE;
}


void StatePromptPostBlind::endPrompt()
{
  CMainWnd::SetStatusText("");

  g_pState = 0;
  if (timer_ != 0)
    ::KillTimer(NULL, timer_);
  timer_ = 0;

  CActionBar* pActionBar = CActionBar::Instance();
  if (pActionBar && ::IsWindow(pActionBar->GetSafeHwnd()))
  {
    pActionBar->stopAnimate();
    pActionBar->enableButtons(0);
  }

  ticks_ = 0;
  turnPtr_.endTurn();
  if (pView_->GetSafeHwnd())
      pView_->UpdateWindow();
}

void StatePromptPostBlind::startPrompt(int player,
                                       const CChips& value,
                                       unsigned int msecs)
{
  BOOL isSevenStud = (Global::GetGameType() == GT_SevenStud);

  endPrompt();

  player_ = player;
  value_ = value;
  ticks_ = 0;
  waitActionTime_ = msecs;
  timeLeft_ = 0;
  timer_ = 0;

  start();
}
