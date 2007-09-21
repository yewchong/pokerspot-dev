/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CFloorMapView
  Created  : 25.10.1999

  OVERVIEW : Implements the floor map view.

 ****************************************************************************/

#include "stdafx.h"
#include "Poker.h"
#include "floormapview.h"
#include "base/draw.h"
#include "ui/global.h"
#include "ui/resource.h"
#include "ui/cmdid.h"
#include "network/server.h"
#include "network/pdulounge.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  const int Margin = 5;
  LPCTSTR g_szJoin = _T("Sit Down");
  const CString g_strTitle(_T("Floor Map"));
}

/////////////////////////////////////////////////////////////////////////////
// CFloorMapView

IMPLEMENT_DYNAMIC(CFloorMapView, CView)

CFloorMapView* CFloorMapView::pInstance_ = 0;

CFloorMapView::CFloorMapView()
  :
  width_     (0),
  height_    (0),
  pLoungeDlg_(0)
{
  pInstance_ = this;
}


CFloorMapView::~CFloorMapView()
{ 
  pInstance_ = 0; 
#ifndef LOUNGE_DLG_
  for (Tables::const_iterator i = tables_.begin(),
                              e = tables_.end();
       i != e;
       ++i)
  {
    delete (*i);
  }
#endif
}


BEGIN_MESSAGE_MAP(CFloorMapView, CView)
  //{{AFX_MSG_MAP(CFloorMapView)
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_WM_LBUTTONUP()
  ON_COMMAND(ID_FAKE_PDUSTATUS, OnFakePdustatus)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUSTATUS, OnUpdateFakePdustatus)
  ON_COMMAND(ID_FAKE_PDUSITTING, OnFakePdusitting)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDUSITTING, OnUpdateFakePdusitting)
  ON_COMMAND(ID_FAKE_PDULOUNGEACCEPT, OnFakePduloungeaccept)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDULOUNGEACCEPT, OnUpdateFakePduloungeaccept)
  ON_COMMAND(ID_FAKE_PDULOGOUTWARNING, OnFakePdulogoutwarning)
  ON_UPDATE_COMMAND_UI(ID_FAKE_PDULOGOUTWARNING, OnUpdateFakePdulogoutwarning)
  ON_COMMAND(ID_JOIN_TABLE, OnJoinTable)
  ON_UPDATE_COMMAND_UI(ID_JOIN_TABLE, OnUpdateJoinTable)
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloorMapView drawing

void CFloorMapView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CFloorMapView diagnostics

#ifdef _DEBUG
void CFloorMapView::AssertValid() const
{
  CView::AssertValid();
}

void CFloorMapView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFloorMapView message handlers


BOOL CFloorMapView::OnEraseBkgnd(CDC* pDC) 
{
  // Use background bitmap or background color
  CRect r;
  GetClientRect(&r);

  if (width_ > 0 && height_ > 0 &&
      bmpBackground_.GetSafeHandle())
  {
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
      CBitmap* pOldBmp = dcMem.SelectObject(&bmpBackground_);

      int w = (r.Width() / width_) + 1;
      int h = (r.Height() / height_) + 1;
      for (int i = 0; i < w; i++)
        for (int j = 0; j < h; j++)
        {
          pDC->BitBlt(i * width_, j * height_,
                      width_, height_,
                      &dcMem, 0, 0,
                      SRCCOPY);
        }

      dcMem.SelectObject(pOldBmp);
    }
  }
  else
  {
    pDC->FillRect(&r, &brBackground_);
  }
  return TRUE;
}


void CFloorMapView::OnSize(UINT nType, int cx, int cy) 
{
  CView::OnSize(nType, cx, cy);
  recalcLayout();
}


void CFloorMapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  AfxMessageBox("OnKeyDown");
  /*
  if (nChar == VK_RETURN)
  {
    if (Global::LoungeServer())
      Global::LoungeServer()->sendLoungeSurveyPDU();
  }
  */
  CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CFloorMapView::OnLButtonUp(UINT nFlags, CPoint point) 
{
  AfxMessageBox("OnLButtonUp");
  CView::OnLButtonUp(nFlags, point);
/*
  Table* pHit = hitTest(point);

  if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
    modifySelection(pHit);
  else
    setSelection(pHit);
*/
}



/////////////////////////////////////////////////////////////////////////////
// CFloorMapView application mfunctions

void CFloorMapView::recalcLayout()
{
#ifndef LOUNGE_DLG_
  if (pLoungeDlg_)
  {
    CRect r;
    GetClientRect(&r);
    pLoungeDlg_->MoveWindow(r.left, r.top,
                            r.Width(), r.Height(),
                            TRUE);
  }
#endif
}


void CFloorMapView::addTable(int number, int players, int lolimit, int hilimit)
{
#ifndef LOUNGE_DLG_
  tables_.push_back(new Table(number, CPoint(10, 10),
                              "Texas Hold'em",
                              players, 10, lolimit, hilimit));
#endif
}


void CFloorMapView::removeTables()
{
  selection_.clear();

#ifndef LOUNGE_DLG_
  for (Tables::const_iterator i = tables_.begin(),
                              e = tables_.end();
       i != e;
       ++i)
  {
    delete (*i);
  }
#endif

  tables_.clear();
}


void CFloorMapView::getTableLimits(USHORT number, USHORT& lolimit, USHORT& hilimit)
{
#ifndef LOUNGE_DLG_
  Table* pT = NULL;
  lolimit = hilimit = 0;

  for (Tables::const_iterator i = tables_.begin(),
                              e = tables_.end();
       i != e;
       ++i)
  {
    if ((*i)->number() == number)
    {
      lolimit = (*i)->lolimit();
      hilimit = (*i)->hilimit();
      break;
    }
  }
#endif
}


Table* CFloorMapView::hitTest(const CPoint& ptHit)
{
#ifndef LOUNGE_DLG_
  Table* pTableHit = 0;

  for (Tables::const_iterator i = tables_.begin(),
                              e = tables_.end();
       i != e;
       ++i)
  {
    Table* pTable = (*i);
    CRect r = pTable->getArea();
    if (r.PtInRect(ptHit))
    {
      pTableHit = pTable;
      break;
    }
  }

  return pTableHit;
#else
  return 0;
#endif
}


void CFloorMapView::setSelection(Table* pTable)
{
  invalidateSelection();
  selection_.clear();
#ifndef LOUNGE_DLG_
  if (pTable)
  {
    selection_.push_back(pTable);
    InvalidateRect(pTable->getArea());
  }
#endif

  updateButtons();
}  

//
// If 'pTable' is already selected, remove it from
// selection.
// If 'pTable' is not selected, add it to selection.
//
void CFloorMapView::modifySelection(Table* pTable)
{
  if (pTable)
  {
#ifndef LOUNGE_DLG_
    InvalidateRect(pTable->getArea());
#endif
    if (isSelected(pTable))
      removeFromSelection(pTable);
    else
      selection_.push_back(pTable);
  }

  updateButtons();
}


void CFloorMapView::removeFromSelection(Table* pTable)
{
  if (pTable)
  {
#ifndef LOUNGE_DLG_
    InvalidateRect(pTable->getArea());
#endif
    Tables::iterator i = std::find(selection_.begin(),
                                   selection_.end(),
                                   pTable);
    if (i != selection_.end())
      selection_.erase(i);
  }

  updateButtons();
}


bool CFloorMapView::isSelected(Table* pTable) const
{
  bool rc = false;

  if (pTable)
  {
    rc = (std::find(selection_.begin(),
                    selection_.end(),
                    pTable) != selection_.end());
  }

  return rc;
}


void CFloorMapView::invalidateSelection()
{
#ifndef LOUNGE_DLG_
  for (Tables::const_iterator i = selection_.begin(),
                              e = selection_.end();
       i != e;
       ++i)
  {
    Table* pTable = (*i);
    CRect r = pTable->getArea();
    InvalidateRect(r);
  }  
#endif
}


//
// Draw "selection handles" around the selected
// objects. Let CRectTracker do the dirty work.
//
void CFloorMapView::drawSelection(CDC* pDC)
{
#ifndef LOUNGE_DLG_
  for (Tables::const_iterator i = selection_.begin(),
                              e = selection_.end();
       i != e;
       ++i)
  {
    Table* pTable = (*i);
    CRect r = pTable->getArea();
    if (pDC->RectVisible(r))
    {
      CRectTracker tracker(&r, CRectTracker::resizeInside);
      tracker.m_nHandleSize = (int)((float)tracker.m_nHandleSize * 1.5f);
      tracker.Draw(pDC); 
    }
  }  
#endif
}


//
// Enable/disable the Join button based 
// on current selection.
//
void CFloorMapView::updateButtons()
{
}


BOOL CFloorMapView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  BOOL rc = FALSE;

  if (LOWORD(wParam) == ID_PDURECEIVED)
  { 
    using namespace Network;
    // All PDUs are handled here
    PDUHeader* pPDU = reinterpret_cast<PDUHeader*>(lParam);

    // Let the PDU implement its logic.
    pPDU->execute(NULL);

    // All PDU's are deallocated here after
    // they have done their job.
    delete pPDU;

    rc = TRUE;
  }

  if (LOWORD(wParam) == IDC_BTNJOIN)
    // ID_BTN_1:
  { // Send Sit Down PDU to sit in a table:
  }

  if (!rc)
    rc = CView::OnCommand(wParam, lParam);

  return rc;
}


BOOL CFloorMapView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  BOOL rc = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

  if (rc)
  {
    setTitle();
  }

  return rc;
}

namespace Network
{
  extern void AddPDUMenuItems(CMenu* pMenu);
}

void CFloorMapView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();
  
  CWnd* pMainFrame = AfxGetMainWnd();
  if (pMainFrame)
  { // Set the Table View menu
    CMenu* pOldMenu = pMainFrame->GetMenu();
    delete pOldMenu;
    CMenu* pNewMenu = new CMenu();
    if (pNewMenu)
      pNewMenu->LoadMenu(IDM_FLOORMAPVIEW);
    pMainFrame->SetMenu(pNewMenu);
  }

#ifndef LOUNGE_DLG_
  pLoungeDlg_ = new CLoungeDlg2(this);
  if (pLoungeDlg_)
  {
    if (pLoungeDlg_->Create(CLoungeDlg2::IDD, this))
    {
      CRect r;
      GetClientRect(&r);
      pLoungeDlg_->MoveWindow(r.left, r.top,
                              r.Width(),
                              r.Height(), FALSE);
      pLoungeDlg_->ShowWindow(SW_NORMAL);
    }
  }
#endif
}

void CFloorMapView::OnFakePdustatus() 
{
}

void CFloorMapView::OnUpdateFakePdustatus(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(tables_.size() == 0);
}

void CFloorMapView::OnFakePdusitting() 
{
#ifndef LOUNGE_DLG_
  if (selection_.begin() != selection_.end())
  {
    Table* pTable = *(selection_.begin());
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_PDURECEIVED, 0),
                reinterpret_cast<LPARAM>(new Network::PDULoungeSitting(
                                                        "x", "x",
                                                        pTable->number(), 100, 1313)));
  }
  else
  {
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_PDURECEIVED, 0),
                reinterpret_cast<LPARAM>(new Network::PDULoungeSitting(
                                                        "x", "x",
                                                        1, 100, 1313)));
  }
#endif
}

void CFloorMapView::OnUpdateFakePdusitting(CCmdUI* pCmdUI) 
{
// Enable always
//  pCmdUI->Enable(selection_.size() > 0);
}

static const CString g_Motd("<HTML>\
<HEAD>\
<TITLE>Message of the Day</TITLE>\
</HEAD>\
<BODY>\
\
<FONT face=\"Times New Roman\" size=2>\
<P><CENTER>This is the <I>Message of the Day</I> dialog!</CENTER></P>\
<P>The contents is displayed in an HTML control embedded in the dialog. The MOTD \
content could also include URL links and graphics.</P>\
<P>Currently this text comes from a <B>local resource</B>; eventually it will be sent by the server \
along with a login accept PDU.</P>\
<P>End of message.</P>\
</FONT>\
\
</BODY>\
</HTML>");


void CFloorMapView::OnFakePduloungeaccept() 
{
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDULoungeAccept(
                                                      "x", "x",
                                                      g_Motd, g_Motd.GetLength())));
}

void CFloorMapView::OnUpdateFakePduloungeaccept(CCmdUI* pCmdUI) 
{
}


void CFloorMapView::setTitle()
{
#ifndef LOUNGE_DLG_
  CWnd* pParentWnd = GetParent();
  if (pParentWnd)
  {
    CString s(CPokerApp::AppName());
#ifdef APPEND_FLOORMAPNAME_
    s += CString(" - ") + g_strTitle;
#endif
    pParentWnd->SetWindowText(s);
  }
#endif
}

void CFloorMapView::OnFakePdulogoutwarning() 
{
  PostMessage(WM_COMMAND,
              MAKEWPARAM(ID_PDURECEIVED, 0),
              reinterpret_cast<LPARAM>(new Network::PDULoungeLogoutWarning("x", "x")));  
}

void CFloorMapView::OnUpdateFakePdulogoutwarning(CCmdUI* pCmdUI) 
{
}

void CFloorMapView::OnJoinTable() 
{
#ifndef LOUNGE_DLG_
  if (Global::LoungeServer())
  {
    for (Tables::const_iterator i = selection_.begin(),
                                e = selection_.end();
         i != e;
         ++i)
    {
      Global::LoungeServer()->sendLoungeSitDownPDU((*i)->number());
    }
  }
#endif
}

void CFloorMapView::OnUpdateJoinTable(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(selection_.size() > 0);
}


void CFloorMapView::OnDestroy() 
{
#ifndef LOUNGE_DLG_
  if (pLoungeDlg_)
  {
    pLoungeDlg_->DestroyWindow();
    delete pLoungeDlg_;
    pLoungeDlg_ = 0;
  }
#endif

  CView::OnDestroy();	
}
