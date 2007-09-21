// promptinputdlg.cpp : implementation file
//

#include "stdafx.h"
#include "ui/promptinputdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPromptInputDlg dialog


CPromptInputDlg::CPromptInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPromptInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPromptInputDlg)
	input_ = _T("");
	prompt_ = _T("");
	//}}AFX_DATA_INIT
}


void CPromptInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPromptInputDlg)
	DDX_Text(pDX, IDC_EDIT1, input_);
	DDX_Text(pDX, IDC_EDIT2, prompt_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPromptInputDlg, CDialog)
	//{{AFX_MSG_MAP(CPromptInputDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPromptInputDlg message handlers
