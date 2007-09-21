/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CWaitDlg
  Created  : 28.10.1999

  OVERVIEW : Dialog for prompting the user to wait until
             new hand start.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/resource.h"
#include "ui/waitdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CWaitDlg application mfunctions

void CWaitDlg::setPrompt(LPCTSTR pszPrompt)
{
  if (::IsWindow(GetSafeHwnd()))
  {
    CWnd* pWnd = GetDlgItem(IDC_PROMPT);
    if (pWnd)
      pWnd->SetWindowText(pszPrompt);
  }
}

BOOL CWaitDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  CenterWindow();	
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWaitDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  BOOL rc = FALSE;
  int cmd = LOWORD(wParam);
  if (cmd == IDCANCEL || cmd == IDOK)
  { // Don't allow closing via ok & cancel
    rc = TRUE;
  }

  if (!rc)
    rc = CDialog::OnCommand(wParam, lParam);

  return rc;
}
