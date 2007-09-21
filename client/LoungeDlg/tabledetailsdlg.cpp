// tabledetailsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lounge.h"
#include "tabledetailsdlg.h"
#include "loungedata.h"
#include "ui/global.h"
#include "network/server.h"
#include "common/chips.h"
#include "common/stringoutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableDetailsDlg dialog


CTableDetailsDlg::CTableDetailsDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog(CTableDetailsDlg::IDD, pParent),
  table_ (0)
{
  //{{AFX_DATA_INIT(CTableDetailsDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CTableDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTableDetailsDlg)
  DDX_Control(pDX, IDC_TD_LIST1, listCtrl_);
  //}}AFX_DATA_MAP
}


BOOL CTableDetailsDlg::PreTranslateMessage(MSG* pMsg)
{
  BOOL rc = FALSE;

  if (pMsg->message == WM_KEYDOWN &&
      pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
  {
    // Eat
    return TRUE;
  }

  rc = CDialog::PreTranslateMessage(pMsg);

  return rc;
}

BEGIN_MESSAGE_MAP(CTableDetailsDlg, CDialog)
  //{{AFX_MSG_MAP(CTableDetailsDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_OBSERVE, OnObserve)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableDetailsDlg message handlers

BOOL CTableDetailsDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  GetDlgItem(IDC_OBSERVE)->EnableWindow(FALSE);

  listCtrl_.InsertColumn(0, "Player", LVCFMT_LEFT, 80, 0);
  listCtrl_.InsertColumn(1, "From", LVCFMT_LEFT, 90, 1);
  listCtrl_.InsertColumn(2, "Chips", LVCFMT_LEFT, 40, 2);
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CTableDetailsDlg::setAvgPot(const CChips& avgpot)
{
  CStrOut s;
  s << avgpot;
  GetDlgItem(IDC_TD_AVGPOT)->SetWindowText(s.str());
}


void CTableDetailsDlg::setFlopPrct(USHORT flopPrct)
{
  CString s;
  s.Format("%d", flopPrct);
  GetDlgItem(IDC_TD_FLOPPRCT)->SetWindowText(s);
}


void CTableDetailsDlg::setQueueSize(USHORT queueSize)
{
/* No queue size - makes no sense since queues are not
   table specific!
  CString s;
  s.Format("%d", queueSize);
  GetDlgItem(IDC_TD_WAITLIST)->SetWindowText(s);
*/
}


void CTableDetailsDlg::setNumPlayers(USHORT numPlayers)
{
  CString s;
  s.Format("%d", numPlayers);
  GetDlgItem(IDC_TD_NUMPLAYERS)->SetWindowText(s);
}


void CTableDetailsDlg::addListItem(const char* player,
                                   const char* city,
                                   const char* chips)
{
  int num_items = listCtrl_.GetItemCount();

  listCtrl_.InsertItem(num_items, player);
  listCtrl_.SetItemText(num_items, 1, city);
  listCtrl_.SetItemText(num_items, 2, chips);
}


void CTableDetailsDlg::setQueryingTitle(USHORT table)
{
  table_ = table;
  CString s;
  s.Format("Table %d Details - querying...", table);
  SetWindowText(s);
  GetDlgItem(IDC_OBSERVE)->EnableWindow(FALSE);

  // If this is a Stud table, need to change from
  // "Flop %" to "2nd Upcard %".
  using namespace Lounge;
  Game& g = GetGameByTable(table);
  if (g.gameType_ == Lounge::GT_SevenStud)
  {
    GetDlgItem(IDC_FLOPPRCNT_PROMPT)->SetWindowText("2nd upcard %:");
  }
  else
  {
    GetDlgItem(IDC_FLOPPRCNT_PROMPT)->SetWindowText("Flop %:");
  }
}


void CTableDetailsDlg::setFinishedTitle()
{
  CString s;
  s.Format("Table %d Details", table_);
  SetWindowText(s);
  GetDlgItem(IDC_OBSERVE)->EnableWindow(TRUE);
}

void CTableDetailsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    CWnd* listctrl = GetDlgItem(IDC_TD_LIST1);
    if (listctrl)
    {
        // resize the list
        CRect r;
        GetClientRect(&r);
        CRect r2;
        listctrl->GetWindowRect(&r2);
        ScreenToClient(&r2);
        r2.right = r.right - 1;
        r2.bottom = r.bottom - 1;
        listctrl->MoveWindow(r2);
    }
}

void CTableDetailsDlg::OnObserve() 
{	
  if (!Global::LoungeServer())
  { // Don't do anything if we don't have live
    // connection to server
    return; 
  }

  Global::LoungeServer()->sendLoungeObservePDU(table_);
}
