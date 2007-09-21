#ifndef __base_statelogin_h__
#define __base_statelogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateLogin
  Created  : 29.10.1999

  OVERVIEW : Implements the log in state that waits
             until new hand starts.

 ****************************************************************************/


#include "base/gamestate.h"

// Application defined
class CWaitDlg;
class CMainWnd;

BEGIN_NAMESPACE(Base)


class StateLogin : public GameState
{
public:
  StateLogin();
  ~StateLogin();

  BOOL tick(DWORD);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  CWaitDlg*   pWaitDlg_;
};


END_NAMESPACE(Base)

#endif