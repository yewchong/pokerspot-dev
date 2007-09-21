/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUNetworkError
  Created  : 16.11.1999

  OVERVIEW : Network Error PDU.
             This PDU is sent by the client itself after a
             fatal network error (i.e., the connection to the
             server was terminated. The action taken here is
             to shut down the client's networking and display
             an error message to the user.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdunetworkerror.h"
#include "network/server.h"
#include "base/gamelogic.h"
#include "ui/global.h"
#ifdef PSPOT_TABLE_MODULE_
  #include "ui/tableview.h"
#else
  #include "loungedlg/floormap.h"
#endif

namespace
{
  LPCTSTR g_szErrFmt = _T("Connection to the server was lost (Error code %i).");
}


namespace Network
{

void PDUNetworkError::execute(Base::GameState*)
{
#ifdef PSPOT_TABLE_MODULE_

  Base::GameLogic::Instance().setNextState(0);
  Global::Instance().closeConnections();

  CTableView* pView = CTableView::Instance();
  if (pView)
  {
    pView->setCaption();
    pView->resetCards();
    pView->announce("");

    for (int i = 0; i < 10; i++)
      pView->removePlayer(i);
    pView->Invalidate();
  }

  // CONNRESET means server closed connection - it's ok
  if (!Global::CloseConnection())//errorCode_ != WSAECONNRESET)
  {
    CString msg;
    msg.Format(g_szErrFmt, errorCode_);
    if (AfxGetMainWnd())
      AfxGetMainWnd()->MessageBox(msg, NULL, MB_OK|MB_ICONEXCLAMATION);
  }

#else

  Global::Instance().closeConnections();

  CFloorMap* pFM = CFloorMap::Instance();
  if (pFM)
    pFM->disconnect();

  if (!Global::CloseConnection())
  {
    CString msg;
    msg.Format(g_szErrFmt, errorCode_);
    if (AfxGetMainWnd())
      AfxGetMainWnd()->MessageBox(msg, NULL, MB_OK|MB_ICONEXCLAMATION);
  }


#endif

}

}