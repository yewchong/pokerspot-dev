#ifndef __base_gamelogic_h__
#define __base_gamelogic_h__

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

#include <list>

// Application defined
class CMainWnd;

namespace Base
{

class GameState;

class GameLogic
{
public:
  GameLogic();
  virtual ~GameLogic();

  static GameLogic& Instance();

  // For tracking bring-in
  static void NewHand();
  static void CardsDealt();
  static BOOL IsBringIn();
  static BOOL GetIsBringIn();

  static BOOL SitOut() { return Instance().sitOut_; }
  static void SitOut(BOOL b);
  static BOOL AutoPostBlind() { return Instance().autoPostBlind_; }
  static void AutoPostBlind(BOOL b) { Instance().autoPostBlind_ = b; }
  static BOOL HideHand() { return Instance().hideHand_; }
  static void HideHand(BOOL);

  static BOOL CallInTurn() { return Instance().callInTurn_; }
  static void CallInTurn(BOOL b) { Instance().callInTurn_ = b; }
  static BOOL RaiseInTurn() { return Instance().raiseInTurn_; }
  static void RaiseInTurn(BOOL b) { Instance().raiseInTurn_ = b; }
  static BOOL FoldInTurn() { return Instance().foldInTurn_; }
  static void FoldInTurn(BOOL b) { Instance().foldInTurn_ = b; }

  static BOOL LeaveTable() { return Instance().leaveTable_; }
  static void LeaveTable(BOOL b) { Instance().leaveTable_  = b; }

  static BOOL TakeFocusOnRequest() { return Instance().takeFocusOnRequest_; }
  static void TakeFocusOnRequest(BOOL b) { Instance().takeFocusOnRequest_ = b; }

  static void SendCallActionPDU();
  static void SendRaiseActionPDU();
  static void SendSpreadRaiseActionPDU();
  static void SendFoldActionPDU();

  static void SendSitOutPDU();
  static void SendSitInPDU();
  static void SendPostBlindPDU();
  static void SendRefuseBlindPDU();

  static void SendShowCardsPDU();
  static void SendHideCardsPDU();

public:
  // One game heartbeat 
  virtual BOOL tick(DWORD dt);
  virtual BOOL draw(CDC*);
  virtual BOOL onCommand(WPARAM, LPARAM);
  virtual BOOL onKey(UINT nChar, UINT nRepCnt, UINT nFlags);

  // Schedule Message of the Day state for execution
  virtual BOOL showMotd();
  // Schedule Floor Map state for execution
  virtual BOOL floorMap();
  // Schedule Table joining state for execution
  virtual BOOL joinTable();
  // Schedule Table leaving state for execution
  virtual BOOL leaveTable();

  // Take next state from state queue for execution
  virtual BOOL nextState();
  // Schedule next state; will be executed after current
  // state has finished.
  virtual BOOL scheduleState(GameState*);
  // Sets next state immediately without scheduling it.
  virtual BOOL setNextState(GameState*);
  // Destroy all pending states.
  virtual BOOL resetStates();

  GameState* currentState() const { return pCurrentState_; }

protected:
  BOOL handlePDU(LPARAM);

  virtual void setState(GameState*);
  virtual void destroyStates();

  // List of scheduled states
  typedef std::list<GameState*> States;
  States states_;

  // Current state
  GameState* pCurrentState_;

private:
  BOOL callInTurn_, raiseInTurn_, foldInTurn_;
  BOOL autoPostBlind_, sitOut_, hideHand_, leaveTable_;
  BOOL takeFocusOnRequest_;

  int bringIn_;
};


} // Base

#endif