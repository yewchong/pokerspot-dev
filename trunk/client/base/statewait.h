#ifndef __base_statewait_h__
#define __base_statewait_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateWait
  Created  : 29.10.1999

  OVERVIEW : Implements the wait state that waits
             until new hand starts.

 ****************************************************************************/


#include "base/gamestate.h"
#include "network/compat.h"

// Application defined
class CWaitDlg;
class CMainWnd;

BEGIN_NAMESPACE(Base)


class StateWait : public GameState
{
public:
  StateWait();
  StateWait(const char* msg);
  ~StateWait();

  BOOL StateWait::tick(DWORD);
  BOOL StateWait::draw(CDC*);
  BOOL onWndMove(int, int);

private:
  CWaitDlg*   pWaitDlg_;
};


END_NAMESPACE(Base)

#endif