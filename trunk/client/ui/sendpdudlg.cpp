// sendpdudlg.cpp : implementation file
//

#include "stdafx.h"
#include "Poker.h"
#include "sendpdudlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendPDUDlg dialog


CSendPDUDlg::CSendPDUDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendPDUDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendPDUDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSendPDUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendPDUDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendPDUDlg, CDialog)
	//{{AFX_MSG_MAP(CSendPDUDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendPDUDlg message handlers
