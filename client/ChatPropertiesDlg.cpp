// ChatPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "poker.h"
#include "ChatPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg dialog


CChatPropertiesDlg::CChatPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChatPropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChatPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatPropertiesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChatPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CChatPropertiesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg message handlers
