/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : PDUNotify
  Created  : 04/03/2000

  OVERVIEW : Notify PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdunotify.h"
#include "network/server.h"
#include "base/misc.h"
#include "ui/global.h"
#ifdef PSPOT_TABLE_MODULE_
#include "ui/tableview.h"
#else
#include "loungedlg/floormap.h"
#endif

BEGIN_NAMESPACE(Network)


void PDUNotify::execute(GameState*)
{
  Global::SetCloseConnection(!(flags_ & NF_NoCloseConnection));

  if (flags_ & NF_StatusMessage)
  {
    // Show message in status bar!
    Base::SetStatusText(NULL, message_);
  }
  else
  { // Show message in dialog
    if (AfxGetMainWnd())
      AfxGetMainWnd()->MessageBox(message_, NULL, MB_OK|MB_ICONINFORMATION);
  }

  if (!(flags_ & NF_NoCloseConnection))
  {
#ifdef PSPOT_TABLE_MODULE_
    if (Global::TableServer())
      Global::Instance().closeTableServer();

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

#else      

    if (Global::LoungeServer())
      Global::Instance().closeLoungeServer();

    CFloorMap* pFM = CFloorMap::Instance();
    if (pFM)
        pFM->disconnect();

#endif
  }
}


END_NAMESPACE(Network)
