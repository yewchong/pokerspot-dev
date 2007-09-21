/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CLoginDlg
  Created  : 27.10.1999

  OVERVIEW : Login dialog class.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/global.h"
#include "ui/logindlg.h"
#include "ui/mainwnd.h"
#ifdef PSPOT_TABLE_MODULE_
  #include "ui/registry.h"
  #include "ui/resource.h"
#else
  #include "loungedlg/resource.h"
  #include "loungedlg/registry.h"
  #include "network/server.h"
#endif
#include "network/network.h"
#include "base/gamelogic.h"
#include "base/cipher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

namespace
{
#ifdef PSPOT_TABLE_MODULE_
  const UINT IDD = IDD_LOGIN;
#else
  const UINT IDD = IDD_LOGIN1;
#endif
}


ULONG CLoginDlg::IpAddress_ = 0;
USHORT CLoginDlg::Port_ = 0;


CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog(IDD, pParent)
{
#ifndef PSPOT_TABLE_MODULE_

  bool b = (Registry::Get1_02(0) == 1);
  Registry::Set1_02(1);

  // Lounge client:
  //{{AFX_DATA_INIT(CLoginDlg)
  if (Registry::GetLoginToPlayMoney(0) != 0)
  {
    host_     = Registry::GetPlayMoneyHost(PSPOT_PLAYMONEY_HOST);
    port_     = Registry::GetPlayMoneyPort(PSPOT_PLAYMONEY_PORT);
  }
  else
  {
    host_     = Registry::GetServerHostName(PSPOT_LOADBALANCER_HOST);
    port_     = Registry::GetServerPort(PSPOT_LOADBALANCER_PORT);
  }
  if (b)
      password_ = Registry::GetPassword("");
  else
      password_ = "";
  if (password_.GetLength() > 0)
  {
    password_ = do_cipher(password_,
                          password_.GetLength(),
                          CO_Decrypt);
  }
  username_ = Registry::GetUsername("");
  //}}AFX_DATA_INIT

#else

  //{{AFX_DATA_INIT(CLoginDlg)
  host_     = "";//Registry::GetServerHostName("cardroom1.pokerspot.com");
  port_     = 0;//Registry::GetServerPort(8501);
  password_ = Registry::GetPassword("");
  if (password_.GetLength() > 0)
  {
    password_ = do_cipher(password_,
                          password_.GetLength(),
                          CO_Decrypt);
  }
  username_ = Registry::GetUsername("");
  //}}AFX_DATA_INIT

#endif

}


CLoginDlg::~CLoginDlg()
{
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_EDITHOST, host_);
    DDX_Text(pDX, IDC_EDITPORT, port_);
	DDX_Text(pDX, IDC_PASSWORD, password_);
	DDV_MaxChars(pDX, password_, 16);
	DDX_Text(pDX, IDC_USERNAME, username_);
	DDV_MaxChars(pDX, username_, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)
	ON_EN_CHANGE(IDC_USERNAME, OnChangeUsername)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

#ifdef PSPOT_TABLE_MODULE_

// This is the Table Client version - used only for testing
void CLoginDlg::OnOK() 
{
  UpdateData(TRUE);

  if (Global::Instance().loginToTableServer(host_,
                                            port_,
                                            username_,
                                            password_))
  {
    CDialog::OnOK();
  }                               
}

#else

// This is the Lounge Client version

void CLoginDlg::OnOK() 
{
  UpdateData(TRUE);

  HWND hWnd = GetParent()->GetSafeHwnd();

  bool loginToPlayMoney =
    (BST_CHECKED == GetDlgItem(IDC_RADIO_PLAYMONEY)->SendMessage(BM_GETCHECK, 0, 0L));
  Registry::SetLoginToPlayMoney(loginToPlayMoney ? 1 : 0);

  if (loginToPlayMoney)
  {
    host_     = Registry::GetPlayMoneyHost(PSPOT_PLAYMONEY_HOST);
    port_     = Registry::GetPlayMoneyPort(PSPOT_PLAYMONEY_PORT);
  }
  else
  {
    host_     = Registry::GetServerHostName(PSPOT_LOADBALANCER_HOST);
    port_     = Registry::GetServerPort(PSPOT_LOADBALANCER_PORT);
  }

  BOOL rc = FALSE;
  if (Global::LoungeServer())
  {
    // This is a relogin attempt
    rc = Global::Instance().loginToLoungeServer(hWnd, 
                                                IpAddress_,
                                                Port_,
                                                username_,
                                                password_);
  }
  else
  {
    Network::Server::Username(username_);
    Network::Server::Password(password_);

    // First login attempt - connect to
    // Lolad Balancer first
    rc = Global::Instance().loginToLoadBalancer(hWnd, 
                                                host_,
                                                port_,
                                                username_);
  }
  if (rc)
  {
    extern void SetPlayMoneyTitle(bool);
    SetPlayMoneyTitle(loginToPlayMoney);

    // Only lounge saves data
    if (BST_CHECKED == GetDlgItem(IDC_REMEMBER_NAMEPW)->SendMessage(BM_GETCHECK, 0, 0L))
    {
      Registry::SetRememberNamePw(1);
      Registry::SetUsername(username_);
      CString s = do_cipher((LPCSTR)password_,
                            password_.GetLength(),
                            CO_Encrypt);
      Registry::SetPassword(s);
    }
    else
    {
      Registry::SetRememberNamePw(0);
      Registry::SetUsername("");
      Registry::SetPassword("");
    }

    CDialog::OnOK();
  }                               
}

#endif


void CLoginDlg::OnChangePassword() 
{
  enableOK();
}


void CLoginDlg::OnChangeUsername() 
{
  enableOK();
}


void CLoginDlg::enableOK()
{
  CString n, p;
  CWnd* pWnd = GetDlgItem(IDC_USERNAME);
  if (pWnd)
    pWnd->GetWindowText(n);
  pWnd = GetDlgItem(IDC_PASSWORD);
  if (pWnd)
    pWnd->GetWindowText(p);

  pWnd = GetDlgItem(IDOK);
  if (pWnd)
    pWnd->EnableWindow(p.GetLength() > 0 && n.GetLength() > 0);
}

BOOL CLoginDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  enableOK();

#ifndef PSPOT_TABLE_MODULE_
  if (Registry::GetRememberNamePw(0))
  {
    GetDlgItem(IDC_REMEMBER_NAMEPW)->SendMessage(BM_SETCHECK,
                                                 MAKEWORD(BST_CHECKED, 0), 0L);
  }

  if (Registry::GetLoginToPlayMoney(0) != 0)
  {
    GetDlgItem(IDC_RADIO_PLAYMONEY)->SendMessage(BM_SETCHECK,
                                                 MAKEWORD(BST_CHECKED, 0), 0L);
  }
  else
  {
    GetDlgItem(IDC_RADIO_REALMONEY)->SendMessage(BM_SETCHECK,
                                                 MAKEWORD(BST_CHECKED, 0), 0L);
  }
#endif
	
  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CLoginDlg::savePassword() const
{
  return FALSE;
/* not a good idea... should at least encrypt it
  ASSERT(GetDlgItem(IDC_SAVEPASSWORD));
  CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_SAVEPASSWORD));
  return pBtn->GetCheck() == 1;
*/
}