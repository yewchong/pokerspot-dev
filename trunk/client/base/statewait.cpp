/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateWait
  Created  : 29.10.1999

  OVERVIEW : Implements the wait state that waits
             until new hand starts.

 ****************************************************************************/

#include "stdafx.h"
#include "base/statewait.h"
#include "base/gamelogic.h"
#include "network/network.h"
#include "ui/mainwnd.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"
#include "ui/waitdlg.h"


BEGIN_NAMESPACE(Base)

using namespace Network;

//
// STATE: Wait for the next event to occur
//
StateWait::StateWait()
  :
  pWaitDlg_(0)
{
  CWnd* pWnd = CTableView::Instance();

  pWaitDlg_ = new CWaitDlg(pWnd);
  pWaitDlg_->Create(IDD_WAITING, pWnd);
  pWaitDlg_->ShowWindow(SW_SHOW);
}


StateWait::StateWait(const char* msg)
  :
  pWaitDlg_(0)
{
  CWnd* pWnd = CTableView::Instance();

  CWnd* pOldFocusWnd = CWnd::GetFocus();

  pWaitDlg_ = new CWaitDlg(pWnd);
  pWaitDlg_->Create(IDD_WAITING, pWnd);
  pWaitDlg_->setPrompt(msg);
  pWaitDlg_->ShowWindow(SW_SHOW);

  // Put the focus back to where it was
  if (pOldFocusWnd)
  {
    pOldFocusWnd->SetFocus();
  }
}


StateWait::~StateWait()
{
  if (pWaitDlg_)
  {
    if (pWaitDlg_->GetSafeHwnd())
        pWaitDlg_->DestroyWindow();
    delete pWaitDlg_;
    pWaitDlg_ = 0;
  }
}


BOOL StateWait::onWndMove(int, int)
{
  if (pWaitDlg_ && pWaitDlg_->GetSafeHwnd())
    pWaitDlg_->CenterWindow();

  return TRUE;
}

BOOL StateWait::tick(DWORD)
{
  return FALSE;
}


BOOL StateWait::draw(CDC*)
{
  return FALSE;
}


END_NAMESPACE(Base)