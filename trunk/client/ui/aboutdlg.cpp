/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CAboutDlg
  Created  : 25.10.1999

  OVERVIEW : About dialog.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/aboutdlg.h"
#include "base/misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAboutDlg::CAboutDlg()
  :
  CDialog(CAboutDlg::IDD)
{
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
  return CDialog::OnEraseBkgnd(pDC);
}


void CAboutDlg::OnPaint() 
{
  CPaintDC dc(this); // device context for painting

  CRect r;
  GetDlgItem(IDC_GROUPBOX_VERSION)->GetWindowRect(&r);
  ScreenToClient(&r);

  CFont f;
  f.CreateStockObject(DEFAULT_GUI_FONT);

  CString text;
  text.Format("Client v0.5\r\n(%s, %s)",
              __DATE__, __TIME__);

  CFont* pOldFont = dc.SelectObject(&f);
  int oldMode = dc.SetBkMode(TRANSPARENT);

  CRect r2(0, 0, 0, 0);
  dc.DrawText("X", 1, &r2, DT_CALCRECT);
  r.OffsetRect(0, r2.Height());

  dc.DrawText(text, text.GetLength(),
              &r, DT_CENTER|DT_WORDBREAK);

  dc.SelectObject(pOldFont);
  dc.SetBkMode(oldMode);
}
