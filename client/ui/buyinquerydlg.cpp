// buyinquerydlg.cpp : implementation file
//

#include "stdafx.h"
#include "ui/buyinquerydlg.h"
#include "ui/tableview.h"
#include "common/stringoutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR g_szPromptFmt = _T("Your account balance is ");
}

/////////////////////////////////////////////////////////////////////////////
// CBuyInQueryDlg dialog



CBuyInQueryDlg::CBuyInQueryDlg(const CChips& limit, CWnd* pParent /*=NULL*/)
  :
  CDialog(CBuyInQueryDlg::IDD, pParent),
  limit_(limit)
{
  CStrOut s;
  s << g_szPromptFmt << limit;

  //{{AFX_DATA_INIT(CBuyInQueryDlg)
  chips_ = limit.asDouble();
  prompt_ = s.str();
  //}}AFX_DATA_INIT
}


void CBuyInQueryDlg::DoDataExchange(CDataExchange* pDX)
{
  CChips lo = 10 * CTableView::Instance()->lolimit();
  if (lo > limit_)
    lo = limit_;

  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CBuyInQueryDlg)
  DDX_Text(pDX, IDC_CHIPS, chips_);
  DDV_MinMaxDouble(pDX, chips_, lo.asDouble(), limit_.asDouble());
  DDX_Text(pDX, IDC_PROMPT, prompt_);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBuyInQueryDlg, CDialog)
	//{{AFX_MSG_MAP(CBuyInQueryDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuyInQueryDlg message handlers
