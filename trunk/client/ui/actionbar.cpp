/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CActionBar
  Created  : 25.10.1999

  OVERVIEW : CActionBar implements the floating action button bar.

 ****************************************************************************/

#include "stdafx.h"
#include "actionbar.h"
#include "global.h"
#include "base/misc.h"
#include "ui/cmdid.h"
#include "ui/resource.h"
#include "ui/mainwnd.h"
#include "ui/tableview.h"
#include "ui/registry.h"
#ifdef PSPOT_TABLE_MODULE_
  #include "base/gamelogic.h"
  #include "base/gamestate.h"
  using Base::GameLogic;
  using Base::GameState;
#endif
#include "common/chips.h"
#include "common/stringoutput.h"


namespace
{

// Check if we're currently prompting for bet action.
// This is true if current state is a bet action state.
BOOL IsPromptingForBetAction()
{
  GameState* state = GameLogic::Instance().currentState();
  if (!state)
    return FALSE;
  return state->onCommand(MAKEWPARAM(ID_ISBETACTION, 0), 0);
}

int GetPayAmount()
{
  GameState* state = GameLogic::Instance().currentState();
  if (!state)
    return 0;
  return state->onCommand(MAKEWPARAM(ID_PDU_GETBET, 0), 0);
}

int g_TournamentTest = -1;
BOOL TournamentTest()
{
    if (g_TournamentTest == -1)
    {
        g_TournamentTest = Registry::GetTournamentTest(0);
    }

    return g_TournamentTest != 0;
}

BOOL g_bSpreadLimit = FALSE;
BOOL g_bHorizontal = TRUE;
BOOL g_bShowToolTips = TRUE;

struct BtnInfo
{
  LPCTSTR text;
  UINT    cmdId;
};

const BtnInfo gBtnInfo[] = {
  { _T("Check"),    ID_BTN_1 },
  { _T("Raise"),    ID_BTN_2 },
  { _T("Fold"),     ID_BTN_3 },
  { _T("Fold"),     ID_BTN_4 },
  { _T("Help..."),  ID_BTN_5 }
};

const int DotSize = 8;

}


/////////////////////////////////////////////////////////////////////////////
// CActionBar dialog

CActionBar* CActionBar::pInst_ = 0;

CActionBar::CActionBar()
  :
  ticks_(0),
  minWidth_(0)
{
  //{{AFX_DATA_INIT(CActionBar)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  brBkgnd_.CreateSolidBrush(RGB(0, 128, 0));

  pInst_ = this;
}


CActionBar::~CActionBar()
{
  pInst_ = 0;
}


BOOL CActionBar::Create(CWnd* pParent)
{
  BOOL rc = FALSE;

  if (Registry::GetActionBarLayout(0) == 1)
  {
    g_bHorizontal = TRUE;
    rc = CDialog::Create(IDD_ACTIONBAR2, pParent);
  }
  else
  {
    g_bHorizontal = FALSE;
    rc = CDialog::Create(IDD_ACTIONBAR, pParent);
  }

  if (rc)
  {
    ASSERT(sizeof(checkBtn_) / sizeof(checkBtn_[0]) == 6);
    checkBtn_[0].SubclassDlgItem(IDC_CHECKPOSTBLIND, this);
    checkBtn_[0].SetButtonStyle(checkBtn_[0].GetButtonStyle()|BS_OWNERDRAW, FALSE);
    checkBtn_[1].SubclassDlgItem(IDC_CHECKSITOUT, this);
    checkBtn_[1].SetButtonStyle(checkBtn_[1].GetButtonStyle()|BS_OWNERDRAW, FALSE);
    checkBtn_[2].SubclassDlgItem(IDC_CHECKFOLD, this);
    checkBtn_[2].SetButtonStyle(checkBtn_[2].GetButtonStyle()|BS_OWNERDRAW, FALSE);
    checkBtn_[3].SubclassDlgItem(IDC_MUCKHAND, this);
    checkBtn_[3].SetButtonStyle(checkBtn_[3].GetButtonStyle()|BS_OWNERDRAW, FALSE);
    if (Registry::GetCheckMuckHand(1) == 1)
    {
      checkBtn_[3].setChecked(TRUE);
      GameLogic::HideHand(TRUE);
    }
    checkBtn_[4].SubclassDlgItem(IDC_CHECKCALL, this);
    checkBtn_[4].SetButtonStyle(checkBtn_[4].GetButtonStyle()|BS_OWNERDRAW, FALSE);
    checkBtn_[5].SubclassDlgItem(IDC_CHECKRAISE, this);
    checkBtn_[5].SetButtonStyle(checkBtn_[5].GetButtonStyle()|BS_OWNERDRAW, FALSE);
  }

  if (rc)
  {
    // Create the action buttons - use the
    // placeholder's size & position
    for (int i = 0; i < (int)NumButtons; i++)
    {
      CRect r;
      GetDlgItem(IDC_BTN_PLACEHOLDER1 + i)->GetWindowRect(&r);
      ScreenToClient(&r);

      UINT style = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW;
  
      if (i == 0)
        style |= BS_DEFPUSHBUTTON;

      if (btn_[i].Create(gBtnInfo[i].text,
                         style, r, this,
                         gBtnInfo[i].cmdId))
      {
        btn_[i].EnableWindow(FALSE);
      }
    }
  }	

  saveWidth();

  return rc;
}


void CActionBar::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CActionBar)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActionBar, CDialog)
  //{{AFX_MSG_MAP(CActionBar)
  ON_WM_CLOSE()
  ON_WM_CONTEXTMENU()
  ON_BN_CLICKED(IDC_CHECKCALL, OnCheckCall)
  ON_BN_CLICKED(IDC_CHECKRAISE, OnCheckRaise)
  ON_BN_CLICKED(IDC_CHECKFOLD, OnCheckFold)
  ON_BN_CLICKED(IDC_CHECKPOSTBLIND, OnCheckPostblind)
  ON_BN_CLICKED(IDC_CHECKSITOUT, OnCheckSitout)
  ON_BN_CLICKED(IDC_MUCKHAND, OnCheckMuckHand)
  ON_WM_CTLCOLOR()
  ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_RAISEAMOUNT, OnChangeRaiseamount)
	//}}AFX_MSG_MAP
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CActionBar message handlers

BOOL CActionBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  BOOL rc = FALSE;
  int cmd = LOWORD(wParam);

  if (cmd >= ID_BTN_1 &&
      cmd <= ID_BTN_5)
  {
    // Button presses need to be routed to view
    CView* pView = Base::GetActiveView();
    if (pView)
      pView->PostMessage(WM_COMMAND, wParam, lParam);
    rc = TRUE;
  } 
  else if (cmd == IDOK || cmd == IDCANCEL)
  { // Don't allow closing via IDOK or IDCANCEL
    rc = TRUE;
  }

  if (!rc)
    rc = CDialog::OnCommand(wParam, lParam);

  return rc;
}


void CActionBar::OnClose() 
{
  // Allow closing through main wnd
  GetParent()->SetFocus();
  GetParent()->PostMessage(WM_CLOSE);
}

void SafeDisable(CDialog* dlg, UINT ctrl)
{
  if (dlg->GetDlgItem(ctrl))
    dlg->GetDlgItem(ctrl)->EnableWindow(FALSE);
}

BOOL CActionBar::OnInitDialog() 
{
  CDialog::OnInitDialog();

  EnableToolTips(TRUE);
  g_bShowToolTips = Registry::GetActionBarTTips(1);

  if (Global::IsObserver())
  {
    SafeDisable(this, IDC_CHECKPOSTBLIND);
    SafeDisable(this, IDC_CHECKCALL);
    SafeDisable(this, IDC_CHECKRAISE);
    SafeDisable(this, IDC_CHECKFOLD);
    SafeDisable(this, IDC_CHECKSITOUT);
    SafeDisable(this, IDC_MUCKHAND);
  }

  if (TournamentTest())
  {
    checkBtn_[4].setChecked(TRUE);
    GameLogic::CallInTurn(TRUE);
  }

  saveWidth();

  if (Global::GetGameType() == GT_SevenStud)
  {
    if (GetDlgItem(IDC_CHECKPOSTBLIND))
      GetDlgItem(IDC_CHECKPOSTBLIND)->SetWindowText("Auto-ante");
  }

  return TRUE;  // return TRUE unless you set the focus to a control
	        // EXCEPTION: OCX Property Pages should return FALSE
}


void CActionBar::ResetAnteCheckbox()
{
  if (!pInst_ || !pInst_->GetDlgItem(IDC_CHECKPOSTBLIND)) return;

  if (Global::GetGameType() == GT_SevenStud)
  {
    pInst_->GetDlgItem(IDC_CHECKPOSTBLIND)->SetWindowText("Auto-ante");
  }
  else
  {
    pInst_->GetDlgItem(IDC_CHECKPOSTBLIND)->SetWindowText("Auto-post blind");
  }
}

void CActionBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{  
/*
  if (!GetDlgItem(ID_BTN_1) ||
      GetDlgItem(ID_BTN_1)->IsWindowEnabled())
  { // If we're in the middle of prompting for an action,
    // don't allow changing the layout
    return;
  }

  CMenu m;
  if (m.CreatePopupMenu())
  {
    m.AppendMenu(MF_STRING, ID_TOGGLEACTIONBARLAYOUT, "Toggle Layout");
    UINT cmd = m.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,
                                point.x, point.y, this);
    if (cmd != 0)
    {
      GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(cmd, 0), 0);
    }
  }	
*/
}


BOOL CActionBar::OnToolTipNotify(UINT id, NMHDR *pNMHDR,
                                 LRESULT *pResult)
{
  if (!g_bShowToolTips)
    return TRUE;

  // need to handle both ANSI and UNICODE versions of the message
  TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
  TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

  static CString strTipText;
  UINT nID = pNMHDR->idFrom;

  if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
  {
    // idFrom is actually the HWND of the tool
    nID = ::GetDlgCtrlID((HWND)nID);
  }
  
  if (nID != 0) // will be zero on a separator
  { 
    CString s;
    if (s.LoadString(nID))
    {
      strTipText = s;
    }
    else
    {
      // XXX TODO ask the current state for a tooltip text
      // For now, hardcoded here
      if (nID == ID_BTN_1)
        strTipText = _T("When this button is enabled, click it to Check or Call");
      else if (nID == ID_BTN_2)
        strTipText = _T("When this button is enabled, click it to Bet or Raise");
      else if (nID == ID_BTN_3)
        strTipText = _T("When this button is enabled, click it to Fold");
    }
  }

  if (pNMHDR->code == TTN_NEEDTEXTA)
  { // This allows for more than 80 characters.
    pTTTA->lpszText = (LPSTR)((LPCTSTR)strTipText);
    //lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
  }
  else
  { // XXX NOT TESTED!!!
    // If the tooltips don't appear properly on Windows NT,
    // then these lines are probably to blaim!!!
    static wchar_t wchar_buf[256];
    _mbstowcsz(wchar_buf, strTipText, sizeof(wchar_buf));
    pTTTW->lpszText = wchar_buf;
    //_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
  }
  *pResult = 0;
  return TRUE;    // message was handled
}


void CActionBar::OnCheckFold() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_CHECKFOLD);
  GameLogic::FoldInTurn(b);
  GetDlgItem(IDC_CHECKCALL)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKCALL)->Invalidate();
  GetDlgItem(IDC_CHECKRAISE)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKRAISE)->Invalidate();

  // For some reason this is called twice per click -
  // don't do the action twice
  static BOOL prevStat = !b;

  // If the player is currently being prompted for an
  // action, simulate it as if the button was pressed
  if (prevStat != b && b &&
      GetDlgItem(ID_BTN_3)->IsWindowEnabled() &&
      IsPromptingForBetAction())
  {
    if (GetPayAmount() == 0)
    { // check
      PostMessage(WM_COMMAND,
                  MAKEWPARAM(ID_BTN_1, 0),
                  0);
    }
    else
    { // fold
      PostMessage(WM_COMMAND,
                  MAKEWPARAM(ID_BTN_3, 0),
                  0);
    }
  }
  prevStat = b;
#endif
}


void CActionBar::OnCheckCall() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_CHECKCALL);
  GameLogic::CallInTurn(b);
  GetDlgItem(IDC_CHECKFOLD)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKFOLD)->Invalidate();
  GetDlgItem(IDC_CHECKRAISE)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKRAISE)->Invalidate();

  // For some reason this is called twice per click -
  // don't do the action twice
  static BOOL prevStat = !b;

  // If the player is currently being prompted for an
  // action, simulate it as if the button was pressed
  if ((prevStat != b) && b &&
      GetDlgItem(ID_BTN_1)->IsWindowEnabled() &&
      IsPromptingForBetAction())
  {
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_BTN_1, 0),
                0);
  }
  prevStat = b;
#endif
}


void CActionBar::OnCheckRaise() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_CHECKRAISE);
  GameLogic::RaiseInTurn(b);
  GetDlgItem(IDC_CHECKCALL)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKCALL)->Invalidate();
  GetDlgItem(IDC_CHECKFOLD)->EnableWindow(!b);
  GetDlgItem(IDC_CHECKFOLD)->Invalidate();

  // For some reason this is called twice per click -
  // don't do the action twice
  static BOOL prevStat = !b;

  // If the player is currently being prompted for an
  // action, simulate it as if the button was pressed
  if ((prevStat != b) && b &&
      GetDlgItem(ID_BTN_2)->IsWindowEnabled() &&
      IsPromptingForBetAction())
  {
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_BTN_2, 0),
                0);
  }
  prevStat = b;
#endif
}


void CActionBar::OnCheckPostblind() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_CHECKPOSTBLIND);
  GameLogic::AutoPostBlind(b);

  // For some reason this is called twice per click -
  // don't do the action twice
  static BOOL prevStat = !b;

  // If the player is currently being prompted for
  // ante/sitout, simulate it as if the ante button was pressed
  if ((prevStat != b) && b &&
      GetDlgItem(ID_BTN_2)->IsWindowEnabled() &&
      IsPromptingForBetAction() == FALSE)
  {
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_BTN_2, 0),
                0);
  }
  prevStat = b;

#endif
}


void CActionBar::OnCheckSitout() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_CHECKSITOUT);
  GameLogic::SitOut(b);

  // For some reason this is called twice per click -
  // don't do the action twice
  static BOOL prevStat = !b;

  // If the player is currently being prompted for
  // ante/sitout, simulate it as if the sitout button was pressed
  if ((prevStat != b) && b &&
      GetDlgItem(ID_BTN_1)->IsWindowEnabled() &&
      IsPromptingForBetAction() == FALSE)
  {
    PostMessage(WM_COMMAND,
                MAKEWPARAM(ID_BTN_1, 0),
                0);
  }
  prevStat = b;


  CWnd* pPostBlind = GetDlgItem(IDC_CHECKPOSTBLIND);
  ASSERT(pPostBlind);
  if (pPostBlind)
  {
    pPostBlind->EnableWindow(!b);
    pPostBlind->Invalidate();
  }
#endif
}


void CActionBar::OnCheckMuckHand() 
{
#ifdef PSPOT_TABLE_MODULE_
  BOOL b = isChecked(IDC_MUCKHAND);
  GameLogic::HideHand(b);
  if (b)
    Registry::SetCheckMuckHand(1);
  else
    Registry::SetCheckMuckHand(0);
#endif    
}


/////////////////////////////////////////////////////////////////////////////
// CActionBar application mfunctions 

void CActionBar::enableButtons(DWORD dwFlags)
{
  if (Global::IsObserver())
  { // Observer can do nothing
    SafeDisable(this, IDC_CHECKPOSTBLIND);
    SafeDisable(this, IDC_CHECKCALL);
    SafeDisable(this, IDC_CHECKRAISE);
    SafeDisable(this, IDC_CHECKFOLD);
    SafeDisable(this, IDC_CHECKSITOUT);
    SafeDisable(this, IDC_MUCKHAND);
    SafeDisable(this, IDC_RAISEAMOUNT);
    SafeDisable(this, IDC_SLIDER1);
    return;
  }

  DWORD orig = dwFlags;

  if (::IsWindow(GetSafeHwnd()))
  {
    for (int i = 0; i < (int)NumButtons; i++)
    {
      btn_[i].EnableWindow(dwFlags & 1);
      btn_[i].Invalidate();
      dwFlags = (dwFlags >> 1);
    }

    if (dwFlags == 0)
    { // all buttons disabled
      GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
      GetDlgItem(IDC_RAISEAMOUNT)->EnableWindow(FALSE);
    }
  }

  // Take focus if
  // - action buttons were enabled
  // - user has the feature on
  // - we didn't have focus already
  if (orig != 0 &&
      GameLogic::TakeFocusOnRequest() &&
      GetForegroundWindow() != AfxGetMainWnd())
  {
    AfxGetMainWnd()->SetForegroundWindow();
    if (GetFocus() != this)
      SetFocus();
  }
}

void CActionBar::setSpread(const CChips& lo, const CChips& hi)
{
    if (!slider_.GetSafeHwnd())
    {
        slider_.SubclassDlgItem(IDC_SLIDER1, this);
        if (!slider_.GetSafeHwnd())
            return; // Should never happen
    }

    if (lo != 0)
    {
        slider_.EnableWindow(TRUE);
        // XXX CChips: this control needs to be
        // adapted to handle cents
        slider_.SetRange(lo.getDollars(), hi.getDollars());
        slider_.SetPos(lo.getDollars());
    }            

   
    CWnd* edit = GetDlgItem(IDC_RAISEAMOUNT);
    if (edit)
    {
        edit->EnableWindow();
        CStrOut s;
        // XXX OLD s.Format("%d", lo);
        s << lo;
        edit->SetWindowText(s.str());
    }
}

CChips CActionBar::getRaise() const
{
    // XXX CChips: this control needs to be
    // adapted to handle cents
    UINT raise = 0;
    CWnd* edit = GetDlgItem(IDC_RAISEAMOUNT);
    if (edit)
    {
        CString s;
        edit->GetWindowText(s);
        raise = atoi(s);
    }
    return CChips(raise, 0);
}


void CActionBar::enableCheckBoxes(BOOL enable)
{
  if (Global::IsObserver())
  { // Observer can do nothing
    SafeDisable(this, IDC_CHECKPOSTBLIND);
    SafeDisable(this, IDC_CHECKCALL);
    SafeDisable(this, IDC_CHECKRAISE);
    SafeDisable(this, IDC_CHECKFOLD);
    SafeDisable(this, IDC_CHECKSITOUT);
    SafeDisable(this, IDC_MUCKHAND);
    SafeDisable(this, IDC_RAISEAMOUNT);
    SafeDisable(this, IDC_SLIDER1);
    return;
  }

  if (::IsWindow(GetSafeHwnd()))
  {
    GetDlgItem(IDC_CHECKPOSTBLIND)->EnableWindow(enable);
    GetDlgItem(IDC_CHECKSITOUT)->EnableWindow(enable);
    GetDlgItem(IDC_CHECKCALL)->EnableWindow(enable);
    GetDlgItem(IDC_CHECKRAISE)->EnableWindow(enable);
    GetDlgItem(IDC_CHECKFOLD)->EnableWindow(enable);
    GetDlgItem(IDC_RAISEAMOUNT)->EnableWindow(enable);
    GetDlgItem(IDC_SLIDER1)->EnableWindow(enable);
  }
}


void CActionBar::ShowToolTips(BOOL b)
{
  g_bShowToolTips = b;
}

//
// Animate those buttons that are enabled.
// For now, a colored dot is rendered on the
// button surface.
// TODO: Replace with a bitmap animation.
//
void CActionBar::animate(DWORD dt)
{
  ticks_ += dt;

  if (ticks_ >= 500)
  {
    ticks_ = 0;

    for (int i = 0; i < (int)NumButtons; i++)
    {
      if (btn_[i].IsWindowEnabled())
      { // Toggle button's highlite status
        btn_[i].setHighLight(!btn_[i].getHighLight());
        btn_[i].Invalidate();
      }
    }
  }
}


void CActionBar::stopAnimate()
{
  for (int i = 0; i < (int)NumButtons; i++)
  {
    btn_[i].setHighLight(FALSE);
    btn_[i].Invalidate();
  }
}


void CActionBar::setButtonText(int i, const CString& s)
{
  ASSERT(::IsWindow(btn_[i].GetSafeHwnd()));
  btn_[i].SetWindowText(s);
  btn_[i].Invalidate();
}


BOOL CActionBar::isChecked(UINT idc)
{
  BOOL rc = FALSE;

  CCheckButton* pBtn = static_cast<CCheckButton*>(GetDlgItem(idc));
  ASSERT_VALID(pBtn);

  if (pBtn)
    rc = (pBtn->isChecked());

  return rc;
}



HBRUSH CActionBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
  return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CActionBar::resetCheckStates()
{
  // this is called when the action bar
  // is recreated. Must redo the check button
  // logic since the enabled/disabled state is
  // not preserved properly.
  if (isChecked(IDC_CHECKSITOUT))
    OnCheckSitout();

  if (isChecked(IDC_CHECKFOLD))
    OnCheckFold();
  else if (isChecked(IDC_CHECKCALL))
    OnCheckCall();
  else if (isChecked(IDC_CHECKRAISE))
    OnCheckRaise();
}


void CActionBar::dock(BOOL b)
{
    if (b)
    {
        ModifyStyle(WS_CAPTION, 0);
        CRect parentRect, r1, r2;
        GetParent()->GetWindowRect(&parentRect);
        GetWindowRect(&r1);
        int ofs = static_cast<CMainWnd*>(GetParent())->getStatusBarHeight();
        ofs += 2 * GetSystemMetrics(SM_CYEDGE) + 2;
        r2.bottom = parentRect.bottom - ofs;
        r2.top = r2.bottom - r1.Height();
        r2.left = parentRect.left;
        r2.right = max(r2.left + minWidth_ + 1,
                       parentRect.left + parentRect.Width());
        r2.top += GetSystemMetrics(SM_CYCAPTION);

        MoveWindow(&r2);
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

BOOL CActionBar::isDocked() const
{
    UINT style = GetStyle();
    return !(style & WS_CAPTION);
}

void CActionBar::attach()
{
    CRect rectTable;
    CTableView::Instance()->GetWindowRect(rectTable);
    CSize s = CTableView::Instance()->getImageSize();

    CRect parentRect, r1, r2;
    GetParent()->GetWindowRect(&parentRect);
    GetWindowRect(&r1);
    int ofs = static_cast<CMainWnd*>(GetParent())->getStatusBarHeight();
    ofs += 2 * GetSystemMetrics(SM_CYEDGE) + 2;
    r2.bottom = parentRect.bottom - ofs;
    r2.top = r2.bottom - r1.Height();
    r2.left = parentRect.left;
    r2.right = parentRect.right;
    MoveWindow(&r2);
}

void CActionBar::saveWidth()
{
    CRect r;
    GetWindowRect(&r);
    minWidth_ = r.Width() + 1;
}


int CActionBar::getMinWidth() const
{
    return minWidth_;
}

void CActionBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if (GetSafeHwnd() && isDocked() &&
        GetDlgItem(IDC_GROUP1))
    {   
        // Move buttons to adapt to new size

        CRect r;
        GetWindowRect(&r);
        int extraWidth = r.Width() - minWidth_;

        if (extraWidth > 0)
        {
            CRect rectGroup;
            GetDlgItem(IDC_GROUP1)->GetWindowRect(&rectGroup);
            CRect rectBar;
            GetWindowRect(&rectBar);

            // This is how much the buttons have room:
            int btnWidth = rectBar.Width() - rectGroup.Width() -
                           2 * GetSystemMetrics(SM_CXEDGE);

            // Each button gets 1/3 of the width
            btnWidth = 0.3f * float(btnWidth);

            btn_[1].GetWindowRect(&r);
            r.left = rectGroup.right + 2;
            r.right = r.left + btnWidth;
            ScreenToClient(&r);
            btn_[1].MoveWindow(r);
            btn_[1].GetWindowRect(&r);


            CRect r2;
            btn_[0].GetWindowRect(&r2);
            const int BtnOffset = 2;
            r2.left = r.right + BtnOffset;
            r2.right = r2.left + btnWidth;
            ScreenToClient(&r2);
            btn_[0].MoveWindow(r2);
            btn_[0].GetWindowRect(&r2);
            
            int edge = GetSystemMetrics(SM_CXEDGE) + GetSystemMetrics(SM_CXFRAME);
            CRect r3;
            btn_[2].GetWindowRect(&r3);
            r3.right = rectBar.right - BtnOffset - edge;
            r3.left = r3.right - btnWidth;
            ScreenToClient(&r3);
            btn_[2].MoveWindow(r3); 

            GetDlgItem(IDC_SLIDER1)->GetWindowRect(&r3);
            r3.right = rectBar.right - BtnOffset - edge;
            ScreenToClient(&r3);
            GetDlgItem(IDC_SLIDER1)->MoveWindow(r3); 
        }
    }	
}

void CActionBar::UncheckActionButtons()
{
    if (TournamentTest()) return;

    if (pInst_ && pInst_->GetSafeHwnd())
        pInst_->uncheckActionButtons();
}

void CActionBar::uncheckActionButtons()
{
    checkBtn_[2].setChecked(FALSE);
    OnCheckFold();
    checkBtn_[4].setChecked(FALSE);
    OnCheckCall();
    checkBtn_[5].setChecked(FALSE);
    OnCheckRaise();
    updateButtons();
}

void CActionBar::UncheckCallButton()
{
    if (TournamentTest()) return;

    if (pInst_ && pInst_->GetSafeHwnd())
        pInst_->uncheckCallButton();   
}

void CActionBar::UncheckRaiseButton()
{
    if (TournamentTest()) return;

    if (pInst_ && pInst_->GetSafeHwnd())
        pInst_->uncheckRaiseButton();
}

void CActionBar::UncheckFoldButton()
{
    if (TournamentTest()) return;

    if (pInst_ && pInst_->GetSafeHwnd())
        pInst_->uncheckFoldButton();
}

void CActionBar::uncheckCallButton()
{
    checkBtn_[4].setChecked(FALSE);
    OnCheckCall();
    updateButtons();
}

void CActionBar::uncheckRaiseButton()
{
    checkBtn_[5].setChecked(FALSE);
    OnCheckRaise();
    updateButtons();
}

void CActionBar::uncheckFoldButton()
{
    checkBtn_[2].setChecked(FALSE);
    OnCheckFold();
    updateButtons();
}

void CActionBar::updateButtons()
{
    CRect r;
    checkBtn_[2].GetWindowRect(&r);
    ScreenToClient(&r);
    InvalidateRect(&r);
    checkBtn_[4].GetWindowRect(&r);
    ScreenToClient(&r);
    InvalidateRect(&r);
    checkBtn_[5].GetWindowRect(&r);
    ScreenToClient(&r);
    InvalidateRect(&r);
}

void CActionBar::CheckSitOut(BOOL b)
{
    if (b && Global::IsTournament())
    {   // Sitting out not allowed in tournaments
        return;
    }

    if (pInst_ && pInst_->GetSafeHwnd())
    {
        pInst_->checkSitOut(b);   
        pInst_->OnCheckSitout();
    }
}

void CActionBar::checkSitOut(BOOL b)
{
    checkBtn_[1].setChecked(b);
    Base::GameLogic::SitOut(b);
    CRect r;
    checkBtn_[1].GetWindowRect(&r);
    ScreenToClient(&r);
    InvalidateRect(&r);
}

void CActionBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

    if (!slider_.GetSafeHwnd())
    {
        slider_.SubclassDlgItem(IDC_SLIDER1, this);
        if (!slider_.GetSafeHwnd())
            return; // Should never happen
    }
    
    int pos = slider_.GetPos();
    if (pos != 0)
    {
        CString s;
        s.Format("%d", pos);
        GetDlgItem(IDC_RAISEAMOUNT)->SetWindowText(s);
        GetDlgItem(ID_BTN_2)->GetWindowText(s);
        if (s.Find("Bet") != -1)
            s.Format("Bet %d", pos);
        else
            s.Format("Raise %d", pos);
        GetDlgItem(ID_BTN_2)->SetWindowText(s);
    }
}


void CActionBar::OnChangeRaiseamount() 
{
    if (!slider_.GetSafeHwnd())
    {
        slider_.SubclassDlgItem(IDC_SLIDER1, this);
        if (!slider_.GetSafeHwnd())
            return; // Should never happen
    }

    CString s;
    GetDlgItem(IDC_RAISEAMOUNT)->GetWindowText(s);
    int pos = atoi(s);
    if (pos != 0)
    {
        GetDlgItem(ID_BTN_2)->GetWindowText(s);
        if (s.Find("Bet") != -1)
            s.Format("Bet %d", pos);
        else
            s.Format("Raise %d", pos);
        GetDlgItem(ID_BTN_2)->SetWindowText(s);
        slider_.SetPos(pos);
    }
}

void CActionBar::setTournament()
{
    // In tournaments blind post and sit-out
    // are disabled
    SafeDisable(this, IDC_CHECKPOSTBLIND);
    SafeDisable(this, IDC_CHECKSITOUT);
}