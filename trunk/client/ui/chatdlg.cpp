/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatDlg
  Created  : 5.11.1999

  OVERVIEW : Dialog class containing the CChatView.

 ****************************************************************************/

#include "stdafx.h"
#include "chatdlg.h"
#include "chatframe.h"
#include "chatview.h"
#include "global.h"
#include "tableview.h"
#include "base/player.h"
#include "ui/cmdid.h"
#include "ui/actionbar.h"
#include "network/server.h"
#include "network/ssl.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  const DWORD MsgSendInterval = 2000;
  const int MaxCharsPerMessage = 100;
}

/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog        


CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog     (CChatDlg::IDD, pParent),
  pChatFrame_ (new CChatFrame()),
  lastMsgTime_(0),
  docking_    (CD_NoDock)
{
  //{{AFX_DATA_INIT(CChatDlg)
  //}}AFX_DATA_INIT
}


CChatDlg::~CChatDlg()
{
}


void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CChatDlg)
  DDX_Control(pDX, IDC_TAB1, tab_);
  //}}AFX_DATA_MAP
}


BOOL CChatDlg::PreTranslateMessage(MSG* pMsg)
{
  // Let ENTER through only if focus is in
  // chat edit field
  if (pMsg->message == WM_KEYDOWN &&
      pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
  {
    // Eat ESC always
    if (pMsg->wParam == VK_ESCAPE)
      return TRUE;

    CWnd* pWnd = GetFocus();
    if (pWnd && (pWnd == GetDlgItem(IDC_EDITCHAT)))
      ; // ok, will pass it through
    else
      return TRUE;
  }

  return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
  //{{AFX_MSG_MAP(CChatDlg)
  ON_WM_SIZE()
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
  ON_WM_NCHITTEST()
  ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
  ON_BN_CLICKED(IDC_RADIO_DEALER, OnRadioDealer)
  ON_BN_CLICKED(IDC_RADIO_USER, OnRadioUser)
  ON_BN_CLICKED(IDC_CHECK_SYSTEMONLY, OnCheckSystemOnly)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDlg message handlers

void CChatDlg::OnSize(UINT nType, int cx, int cy) 
{ 
  // Rearrange the child controls

  CDialog::OnSize(nType, cx, cy);
  if (pChatFrame_ && ::IsWindow(pChatFrame_->GetSafeHwnd()))
  {
    CRect rectClient, rectEdit, rectTab;
    GetClientRect(&rectClient);

    int ofs = 2;
    GetDlgItem(IDC_EDITCHAT)->GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);
    rectEdit.right = rectClient.right - GetSystemMetrics(SM_CXBORDER);
    GetDlgItem(IDC_EDITCHAT)->MoveWindow(rectEdit);

    GetDlgItem(IDC_TAB1)->GetWindowRect(&rectTab);
    ScreenToClient(&rectTab);
    CRect rectItem;
    static_cast<CTabCtrl*>(GetDlgItem(IDC_TAB1))->GetItemRect(0, &rectItem);
    rectTab.left = rectClient.left;
    rectTab.right = rectClient.right;
    rectTab.top = rectClient.bottom - rectItem.Height();
    rectTab.bottom = rectClient.bottom;

//#ifndef PSPOT_TABLE_MODULE_
    // lounge does not have tab => chat
    // fills entire client area
    rectTab.top = rectClient.bottom;
//#endif

    rectClient.top = rectEdit.bottom;  
    rectClient.bottom = rectTab.top;
    pChatFrame_->MoveWindow(rectClient);
  }
}


BOOL CChatDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  BOOL rc = CDialog::Create(IDD, pParentWnd);

  if (rc)
  { // Show an initial message showing
    // whether SSL is in use
    if (PSI_SSL_IsEnabled())
    {
      PDU_DEBUG("*** SSL security ENABLED ***");
    }
    else
    {
      PDU_DEBUG("*** SSL security DISABLED ***");
    }
  }

  return rc;
}


BOOL CChatDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  CRect r(0, 0, 0, 0);
  GetClientRect(&r);
	
  if (pChatFrame_)
    pChatFrame_->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, r, this);

  CRect rectClient, rectEdit;
  GetClientRect(&rectClient);

  CWnd* pEditChat = GetDlgItem(IDC_EDITCHAT);
  if (pEditChat)
  {
    pEditChat->SendMessage(EM_LIMITTEXT, (WPARAM)MaxCharsPerMessage, 0);
    pEditChat->GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);
    rectEdit.right = rectClient.right;
    rectClient.top = rectEdit.bottom;  
    pChatFrame_->MoveWindow(rectClient);
  }

#ifdef PSPOT_TABLE_MODULE_
  // Lounge does not have radios
  GetDlgItem(IDC_RADIO_ALL)->SendMessage(BM_SETCHECK,
                                         MAKEWORD(BST_CHECKED, 0), 0L);

#endif


  if (pChatFrame_)
  {
    // Activate the first tab 
    //tab_.SetCurSel(0);
    CChatView* pView = static_cast<CChatView*>(pChatFrame_->GetActiveView());
    ASSERT_VALID(pView);
    pView->setCurrentView(0);
  }
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CChatDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  UINT cmd = LOWORD(wParam);

  BOOL rc = FALSE;

  if (cmd == ID_PDURECEIVED)
  { // Chat PDUs may end up here.
    using namespace Network;
    PDUHeader* pPDU = reinterpret_cast<PDUHeader*>(lParam);
    if (pPDU)
    { // Let the PDU implement its logic.
      pPDU->execute(NULL);
      // All PDU's are deallocated after
      // they have done their job.
      delete pPDU;
    }
    rc = TRUE;
  }
  else if (cmd == IDOK || cmd == IDCANCEL)
  {
    CWnd* pWnd = GetFocus();
    if (pWnd && (pWnd == GetDlgItem(IDC_EDITCHAT)))
    {
      CString s = Global::Instance().chatEnabledMsg();
      if (s.GetLength() > 0)
      {
        AfxMessageBox(s, MB_ICONINFORMATION);
      }
      else
      {
        CString msg;
        pWnd->GetWindowText(msg);
        if (msg.GetLength() > 0)
        {
          if (!checkSendTime())
          { // Don't allow the user to flood w/ chats
            MessageBeep(-1);
          }
          else
          {
            pWnd->SetWindowText(_T(""));
#ifdef PSPOT_TABLE_MODULE_
            if (Global::TableServer())
              Global::TableServer()->sendChatPDU(msg);
#else     
            if (Global::LoungeServer())
              Global::LoungeServer()->sendChatPDU(msg);
#endif
          }
        }
      }
    }
    rc = TRUE; // Prevent CDialog from closing!
  }

  if (!rc)
    rc = CDialog::OnCommand(wParam, lParam);

  return rc;
}


void CChatDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
  *pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CChatDlg application mfunctions


bool CChatDlg::checkSendTime()
{
  bool allowSend = true;
  DWORD now = ::timeGetTime();

  if (lastMsgTime_ == 0)
    lastMsgTime_ = (now - MsgSendInterval - 1);

  if (now - lastMsgTime_ < MsgSendInterval)
    allowSend = false;
  else
    lastMsgTime_ = now;

  return allowSend;
}


// Lounge has no tabs, allow hiding
void CChatDlg::hideTabs()
{
}


UINT CChatDlg::OnNcHitTest(CPoint point)
{
  return CDialog::OnNcHitTest(point);
/*
  UINT nRet = CDialog::OnNcHitTest(point);
  switch (nRet)
  {
    case HTTOP:  
        return HTTOP;
    case HTBOTTOM:
        return HTBOTTOM;
   case HTLEFT:
        return HTLEFT;
    case HTRIGHT:
        return HTRIGHT;

    case HTTOPLEFT:
        return  HTTOPLEFT;
    case HTBOTTOMLEFT:
        return HTBOTTOMLEFT;

    case HTTOPRIGHT:
        return HTTOPRIGHT;
    case HTBOTTOMRIGHT:
        return HTBOTTOMRIGHT;

    default:
        return HTCAPTION;
   }

  return nRet;
*/
}

void CChatDlg::OnRadioAll() 
{
    CChatView* pView = static_cast<CChatView*>(pChatFrame_->GetActiveView());
    ASSERT_VALID(pView);
    pView->setCurrentView(0);
}

void CChatDlg::OnRadioDealer() 
{
    CChatView* pView = static_cast<CChatView*>(pChatFrame_->GetActiveView());
    ASSERT_VALID(pView);
    pView->setCurrentView(1);
}

void CChatDlg::OnRadioUser() 
{
    CChatView* pView = static_cast<CChatView*>(pChatFrame_->GetActiveView());
    ASSERT_VALID(pView);
    pView->setCurrentView(2);
}

void CChatDlg::OnCheckSystemOnly() 
{
    CChatView* pView = static_cast<CChatView*>(pChatFrame_->GetActiveView());
    ASSERT_VALID(pView);

    if (GetDlgItem(IDC_CHECK_SYSTEMONLY))
    {
      BOOL isChecked =
        BST_CHECKED == GetDlgItem(IDC_CHECK_SYSTEMONLY)->SendMessage(BM_GETCHECK, 0, 0);

      if (isChecked)
        pView->setCurrentView(1);
      else
        pView->setCurrentView(0);
    }
}


void CChatDlg::dock(ChatDlg_DockMode dm)
{
    docking_ = dm;

    if (dm == CD_DockLeft || dm == CD_DockRight)
    {
        // take caption away
        ModifyStyle(WS_CAPTION, 0);

        CRect r1;
        GetWindowRect(&r1);
        r1.top += GetSystemMetrics(SM_CYCAPTION);
        MoveWindow(&r1);
    }
    else
    {
        ModifyStyle(0, WS_CAPTION);

        CRect r1;
        GetWindowRect(&r1);
        r1.top -= GetSystemMetrics(SM_CYCAPTION);
        MoveWindow(&r1);
    }
}

CChatDlg::ChatDlg_DockMode CChatDlg::isDocked() const
{
    return docking_;
}

void CChatDlg::attach()
{
#ifdef PSPOT_TABLE_MODULE_
    const int MinWidth = 120;
    if (docking_ == CD_DockRight)
    {
        CRect rectTable, r1;
        CTableView::Instance()->GetWindowRect(rectTable);
        CSize s = CTableView::Instance()->getImageSize();
        rectTable.right = rectTable.left + s.cx;
        rectTable.bottom = rectTable.top + s.cy;
        GetWindowRect(&r1);
        int width = r1.Width();
        r1.left = rectTable.right + GetSystemMetrics(SM_CXEDGE);
        r1.right = r1.left + width;
        r1.top = rectTable.top;
        r1.bottom = rectTable.bottom;

        if (r1.right > GetSystemMetrics(SM_CXSCREEN))
            r1.right = GetSystemMetrics(SM_CXSCREEN);

        if (r1.Width() < MinWidth)
            r1.right = r1.left + MinWidth;

        MoveWindow(&r1);
    }
    else if (docking_ == CD_DockLeft)
    {
        CRect rectTable, r1;
        CTableView::Instance()->GetWindowRect(rectTable);
        CSize s = CTableView::Instance()->getImageSize();
        rectTable.bottom = rectTable.top + s.cy;
        GetWindowRect(&r1);
        int width = r1.Width();
        r1.left = rectTable.left - width;// - GetSystemMetrics(SM_CXEDGE);
        r1.right = r1.left + width;
        r1.top = rectTable.top;
        r1.bottom = rectTable.bottom;

        if (r1.left < 0)
            r1.left = 0;

        if (r1.Width() < MinWidth)
            r1.left = r1.right - MinWidth;

        MoveWindow(&r1);
    }
#endif
}

int CChatDlg::getMinWidth() const
{
    return 100;
}