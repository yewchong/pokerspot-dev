/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdDlg
  Created  : 25.10.1999

  OVERVIEW : Message of the day dialog.

 ****************************************************************************/

#include "stdafx.h"
#include "motddlg.h"
#include "motdframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotdDlg dialog        


CMotdDlg::CMotdDlg(LPCTSTR pszMotdMessage,
                   LPCTSTR pszCaption,
                   CWnd* pParent /*=NULL*/)
  :
  CDialog    (CMotdDlg::IDD, pParent),
  pHTMLFrame_(new CMotdFrame(pszMotdMessage)),
  caption_   (pszCaption)
{  
	//{{AFX_DATA_INIT(CMotdDlg)
	//}}AFX_DATA_INIT
}


CMotdDlg::~CMotdDlg()
{
}


void CMotdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMotdDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMotdDlg, CDialog)
  //{{AFX_MSG_MAP(CMotdDlg)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotdDlg message handlers

void CMotdDlg::OnSize(UINT nType, int cx, int cy) 
{
  const int XPaneMargin = 14;
  const int YPaneMargin = 38;
  const int YButtonMargin = 6;
 
  CDialog::OnSize(nType, cx, cy);

  CRect r;
  GetClientRect(&r);

  CWnd* pWnd = GetDlgItem(IDC_MESSAGEPANE);
  if (pWnd)
  {
    CRect rectMsgPane;
    pWnd->GetWindowRect(&rectMsgPane);
    ScreenToClient(&rectMsgPane);
    rectMsgPane.right = r.right - XPaneMargin;
    rectMsgPane.bottom = r.bottom - YPaneMargin;
    pWnd->MoveWindow(rectMsgPane);

    if (pHTMLFrame_ && ::IsWindow(pHTMLFrame_->GetSafeHwnd()))
      pHTMLFrame_->MoveWindow(rectMsgPane);
  }

  pWnd = GetDlgItem(IDOK);
  if (pWnd)
  {
    CRect rBtn;
    pWnd->GetWindowRect(&rBtn);
    ScreenToClient(&rBtn);
    pWnd->MoveWindow(r.left + (r.Width() / 2) - (rBtn.Width() / 2),
                     r.bottom - rBtn.Height() - YButtonMargin,
                     rBtn.Width(), rBtn.Height());                      
  }
	
}


BOOL CMotdDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
  return CDialog::Create(IDD, pParentWnd);
}


BOOL CMotdDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  SetWindowText(caption_);

  CRect r(0, 0, 0, 0);
  CWnd* pWnd = GetDlgItem(IDC_MESSAGEPANE);
  if (pWnd)
  {
    pWnd->GetWindowRect(&r);
    ScreenToClient(&r);
  }
	
  if (pHTMLFrame_)
    pHTMLFrame_->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, r, this);
	
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CMotdDlg application mfunctions
