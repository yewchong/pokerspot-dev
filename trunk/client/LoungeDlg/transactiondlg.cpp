// transactiondlg.cpp : implementation file
//

#include "stdafx.h"
#include "loungedlg/lounge.h"
#include "loungedlg/transactiondlg.h"
#include "loungedlg/progind.h"
#include "loungedlg/filler.h"
#include "ui/global.h"
#include "ui/cmdid.h"
#include "network/server.h"
#include "network/pduloungetransactionreply.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransactionDlg dialog


CTransactionDlg::CTransactionDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog     (CTransactionDlg::IDD, pParent),
  timer_      (0),
  lastTime_   (0),
  queriesSent_(0)
{
	//{{AFX_DATA_INIT(CTransactionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	progressInd_ = new CProgressIndicator; 
}


CTransactionDlg::~CTransactionDlg()
{
}
    
void CTransactionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransactionDlg, CDialog)
	//{{AFX_MSG_MAP(CTransactionDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDlg message handlers

BOOL CTransactionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  timer_ = SetTimer(0, 250, NULL);

	CWnd* pWnd = GetDlgItem(IDC_PROGRESS1);
	ASSERT(pWnd);
	CRect r;
	pWnd->GetWindowRect(r);
	ScreenToClient(r);
	VERIFY(progressInd_->Create(NULL, "PI1",  WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE, 
		                          r, this, IDC_PROGRESS1));

  progressInd_->SetPos(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransactionDlg::OnDestroy() 
{
	CDialog::OnDestroy();

  if (timer_ != 0)
  {
    KillTimer(timer_);
    timer_ = 0;
  }

	delete progressInd_;
  progressInd_ = NULL;
}

BOOL CTransactionDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  // Don't allow closing the dialog with
  // enter or cancel
  return TRUE;
}

void CTransactionDlg::OnTimer(UINT nIDEvent) 
{
  int pos = progressInd_->GetPos();
  
  static bool b = true;
  if (pos == 10)
  {
    if (b)
      progressInd_->SetFiller(new CRToLFiller);
    else
      progressInd_->SetFiller(new CLToRFiller);
    pos = 0;
    b = !b;
  }
  pos += 1;
  progressInd_->SetPos(pos);
  
  long now = 0;
  time(&now);
  if (now - lastTime_ > 15)
  {   
    // Send a transaction status query every 15 seconds
    
    if (++queriesSent_ > 6)
    {   // Minute and a half has passed, no reply - quit
      KillTimer(timer_);
      timer_ = 0;
      // Notify parent so it will close this dialog
      GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(IDC_PROGRESS1, 0), 0);
      return;
    }
    
    if (Global::LoungeServer())
      Global::LoungeServer()->sendTransactionQueryPDU();
    lastTime_ = now;
  }
  
  CDialog::OnTimer(nIDEvent);
}
