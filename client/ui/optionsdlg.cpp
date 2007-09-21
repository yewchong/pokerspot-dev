// optionsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ui/optionsdlg.h"
#include "ui/actionbar.h"
#include "ui/chatdlg.h"
#include "ui/cmdid.h"
#include "ui/soundmanager.h"
#include "ui/registry.h"
#include "base/gamelogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(COptionsDlg)
  //}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(COptionsDlg)
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_SETTINGS_HORZACTIONBAR, OnCheckSettingsHorzactionbar)
	ON_BN_CLICKED(IDC_SETTINGS_SOUNDS, OnCheckSettingsSounds)
	ON_BN_CLICKED(IDC_SETTINGS_TTIPS, OnCheckSettinsTtips)
	ON_BN_CLICKED(IDC_TAKEFOCUS, OnCheckSettingsTakefocus)
	ON_BN_CLICKED(IDC_SETTINGS_DOCKACTIONBAR, OnSettingsDockactionbar)
	ON_BN_CLICKED(IDC_SETTINGS_DOCKCHATWND, OnSettingsDockchatwnd)
	ON_BN_CLICKED(IDC_SETTINGS_DOCKCHATLEFT, OnSettingsDockchatleft)
	//}}AFX_MSG_MAP
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnCheckSettingsHorzactionbar() 
{
  GetParent()->PostMessage(WM_COMMAND,
                           MAKEWPARAM(ID_TOGGLEACTIONBARLAYOUT, 0),
                           0);
  GetDlgItem(IDC_SETTINGS_DOCKACTIONBAR)->SendMessage(BM_SETCHECK,
                                                        MAKEWORD(BST_UNCHECKED, 0), 0L);
}

void COptionsDlg::OnSettingsDockactionbar() 
{
  GetParent()->PostMessage(WM_COMMAND,
                           MAKEWPARAM(ID_TOGGLEACTIONBARDOCK, 0),
                           0);
}

void COptionsDlg::OnSettingsDockchatwnd() 
{
  BOOL isChecked =
    BST_CHECKED == GetDlgItem(IDC_SETTINGS_DOCKCHATWND)->SendMessage(BM_GETCHECK, 0, 0);

  int mode = isChecked ? CChatDlg::CD_DockRight : 0;
  GetParent()->PostMessage(WM_COMMAND,
                           MAKEWPARAM(ID_TOGGLECHATWNDDOCK, mode),
                           0);

  if (isChecked)
  {
    GetDlgItem(IDC_SETTINGS_DOCKCHATLEFT)->SendMessage(BM_SETCHECK,
                                                      MAKEWORD(BST_UNCHECKED, 0), 0L);
  }
}

void COptionsDlg::OnSettingsDockchatleft() 
{
  BOOL isChecked =
    BST_CHECKED == GetDlgItem(IDC_SETTINGS_DOCKCHATLEFT)->SendMessage(BM_GETCHECK, 0, 0);

  int mode = isChecked ? CChatDlg::CD_DockLeft : 0;

  GetParent()->PostMessage(WM_COMMAND,
                           MAKEWPARAM(ID_TOGGLECHATWNDDOCK, mode),
                           0);

  if (isChecked)
  {
    GetDlgItem(IDC_SETTINGS_DOCKCHATWND)->SendMessage(BM_SETCHECK,
                                                      MAKEWORD(BST_UNCHECKED, 0), 0L);
  }
}

void COptionsDlg::OnCheckSettingsSounds() 
{
  BOOL isChecked =
    BST_CHECKED == GetDlgItem(IDC_SETTINGS_SOUNDS)->SendMessage(BM_GETCHECK, 0, 0);

  Registry::SetSoundEnabled(isChecked ? 1 : 0);
  SndMan::Instance()->enable(isChecked);
}

void COptionsDlg::OnCheckSettinsTtips() 
{
  int isChecked =
    BST_CHECKED == GetDlgItem(IDC_SETTINGS_TTIPS)->SendMessage(BM_GETCHECK, 0, 0);

  Registry::SetActionBarTTips(isChecked ? 1 : 0);
  CActionBar::ShowToolTips(isChecked);
}

void COptionsDlg::OnCheckSettingsTakefocus() 
{
  int isChecked =
    BST_CHECKED == GetDlgItem(IDC_TAKEFOCUS)->SendMessage(BM_GETCHECK, 0, 0);

  Base::GameLogic::TakeFocusOnRequest(isChecked ? 1 : 0);
  Registry::SetSettingsTakeFocus(isChecked ? 1 : 0);
}

BOOL COptionsDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  EnableToolTips(TRUE);

  int layout = Registry::GetActionBarLayout(0);
  if (layout == 1)
  {
    GetDlgItem(IDC_SETTINGS_HORZACTIONBAR)->SendMessage(BM_SETCHECK,
                                                        MAKEWORD(BST_CHECKED, 0), 0L);
  }

  int dock = Registry::GetActionBarDock(DefaultActionBarDock);
  if (dock == 1)
  {
    GetDlgItem(IDC_SETTINGS_DOCKACTIONBAR)->SendMessage(BM_SETCHECK,
                                                        MAKEWORD(BST_CHECKED, 0), 0L);
  }

  dock = Registry::GetChatWndDock(DefaultChatWndDock);
  if (dock == 1)
  {
    GetDlgItem(IDC_SETTINGS_DOCKCHATWND)->SendMessage(BM_SETCHECK,
                                                      MAKEWORD(BST_CHECKED, 0), 0L);
  }
  else if (dock == 2)
  {
    GetDlgItem(IDC_SETTINGS_DOCKCHATLEFT)->SendMessage(BM_SETCHECK,
                                                      MAKEWORD(BST_CHECKED, 0), 0L);
  }

  int soundson = Registry::GetSoundEnabled(1);
  if (soundson == 1)
  {
    GetDlgItem(IDC_SETTINGS_SOUNDS)->SendMessage(BM_SETCHECK,
                                                 MAKEWORD(BST_CHECKED, 0), 0L);
  }

  int ttips = Registry::GetActionBarTTips(1);
  if (ttips == 1)
  {
    GetDlgItem(IDC_SETTINGS_TTIPS)->SendMessage(BM_SETCHECK,
                                                MAKEWORD(BST_CHECKED, 0), 0L);
  }

  int takefocus = Base::GameLogic::TakeFocusOnRequest();
  if (takefocus == 1)
  {
    GetDlgItem(IDC_TAKEFOCUS)->SendMessage(BM_SETCHECK,
                                           MAKEWORD(BST_CHECKED, 0), 0L);
  }

  CString s = Registry::GetHandLogEmailAddress("");
  GetDlgItem(IDC_SETTINGS_EMAILADDRESS)->SetWindowText(s);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK() 
{
  CString s;
  GetDlgItem(IDC_SETTINGS_EMAILADDRESS)->GetWindowText(s);
  Registry::SetHandLogEmailAddress(s);
  CDialog::OnOK();
}

BOOL COptionsDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR,
                                  LRESULT *pResult)
{
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
    s.LoadString(nID);
    strTipText = s;
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

