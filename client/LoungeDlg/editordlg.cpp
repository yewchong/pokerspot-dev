// editordlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lounge.h"
#include "editordlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg dialog

CEditorDlg* CEditorDlg::pInst_ = 0;

CEditorDlg::CEditorDlg(CWnd* pParent /*=NULL*/)
	:
    CDialog(CEditorDlg::IDD, pParent),
    xofs_(0), yofs_(0)
{
	//{{AFX_DATA_INIT(CEditorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    pInst_ = this;
}


CEditorDlg::~CEditorDlg()
{
    pInst_ = NULL;
}


void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CEditorDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg message handlers

BOOL CEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    SetWindowText("Fetching data...");

    CRect r;
    GetDlgItem(IDC_EDIT)->GetWindowRect(&r);

    CRect r2;
    GetWindowRect(&r2);
    xofs_ = r2.right - r.right;
    yofs_ = r2.bottom - r.bottom;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorDlg::OnSize(UINT nType, int cx, int cy) 
{
    if (!GetDlgItem(IDC_EDIT))
    {
        // First OnSize comes before child controls exist
        CDialog::OnSize(nType, cx, cy);
        return;
    }

	CDialog::OnSize(nType, cx, cy);

    CRect r1(0, 0, 0, 0);
    GetWindowRect(&r1);

    CRect r2(0, 0, 0, 0);
    GetDlgItem(IDC_EDIT)->GetWindowRect(&r2);

    int oldx = r2.right;
    int oldy = r2.bottom;
    r2.right = r1.right - xofs_;
    r2.bottom = r1.bottom - yofs_;
    int xdiff = r2.right - oldx;
    int ydiff = r2.bottom - oldy;

    ScreenToClient(&r2);
    GetDlgItem(IDC_EDIT)->MoveWindow(r2);

    GetDlgItem(IDOK)->GetWindowRect(&r1);
    r1.OffsetRect(0, ydiff);
    ScreenToClient(&r1);
    GetDlgItem(IDOK)->MoveWindow(&r1);

    GetDlgItem(IDCANCEL)->GetWindowRect(&r1);
    r1.OffsetRect(0, ydiff);
    ScreenToClient(&r1);
    GetDlgItem(IDCANCEL)->MoveWindow(&r1);
}

CEditorDlg* CEditorDlg::Inst()
{
    return pInst_;
}

void CEditorDlg::setText(const char* t)
{
    if (GetDlgItem(IDC_EDIT))
    {
        GetDlgItem(IDC_EDIT)->SetWindowText(t);
    }
}