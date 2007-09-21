// cashierdlg.cpp : implementation file
//

#include "stdafx.h"
#include "loungedlg/Lounge.h"
#include "loungedlg/cashierdlg.h"
#include "loungedlg/loungedlg.h"
#include "loungedlg/transactiondlg.h"
#include "loungedlg/creditcarddlg.h"
#include "base/draw.h"
#include "base/misc.h"
#include "network/server.h"
#include "ui/global.h"
#include "common/chips.h"
#include "common/stringoutput.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCashierDlg* CCashierDlg::pDlg_ = 0;
CChips CCashierDlg::Balance_; 
bool CCashierDlg::known_ = false; // Balance initially not known


CCashierDlg::CCashierDlg(CWnd* pParent /*=NULL*/)
  :
  CDialog(CCashierDlg::IDD, pParent),
  pImage_(0),
  pTransactionDlg_(0)
{
  //{{AFX_DATA_INIT(CCashierDlg)
  //}}AFX_DATA_INIT

  pImage_ = Base::JPGImage::LoadImage("cashier.jpg");
  if (!pImage_)
  {
    AfxMessageBox("Unable to load cashier graphics: cashier.jpg");
  }

  pDlg_ = this;
}


CCashierDlg::~CCashierDlg()
{
  delete pTransactionDlg_;
  pTransactionDlg_ = 0;
  delete pImage_;
  pImage_ = 0;
  pDlg_ = 0;
}

void CCashierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCashierDlg)
	DDX_Control(pDX, IDC_CREDITCARD, creditCardCombo_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCashierDlg, CDialog)
	//{{AFX_MSG_MAP(CCashierDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUYIN, OnBuyin)
	ON_BN_CLICKED(IDC_CASHOUT, OnCashout)
	ON_EN_CHANGE(IDC_BUYINAMOUNT, OnChangeBuyinamount)
	ON_CBN_SELCHANGE(IDC_CREDITCARD, OnSelchangeCreditcard)
	ON_EN_CHANGE(IDC_CASHOUTAMOUNT, OnChangeCashoutamount)
	ON_BN_CLICKED(IDC_COMMIT, OnCommit)
	ON_CBN_EDITCHANGE(IDC_CREDITCARD, OnEditchangeCreditcard)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CREDITCARDS, OnCreditcards)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCashierDlg message handlers

void CCashierDlg::OnPaint() 
{
  CPaintDC dc(this);
  if (pImage_)
  {
    CRect r;
    GetClientRect(&r);
    pImage_->drawImage(dc, r);

    int oldMode = dc.SetBkMode(TRANSPARENT);

    BYTE red = 190, green = 160, blue = 70, alpha = 160;
    COLORREF light = RGB(min(255, 1.7f * float(red)),
                         min(255, 1.7f * float(green)),
                         min(255, 1.7f * float(blue)));
    COLORREF dark = RGB(max(0, 0.4f * float(red)),
                        max(0, 0.4f * float(green)),
                        max(0, 0.4f * float(blue)));

    GetDlgItem(IDC_CASHIER_GROUP1)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawRectAlpha(&dc, r, red, green, blue, alpha);
    Base::DrawBumpRect(&dc, r, dark, light);

    GetDlgItem(IDC_CASHIER_GROUP2)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawRectAlpha(&dc, r, red, green, blue, alpha);
    Base::DrawBumpRect(&dc, r, dark, light);

    GetDlgItem(IDC_CASHIER_GROUP3)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawRectAlpha(&dc, r, red, green, blue, alpha);
    Base::DrawBumpRect(&dc, r, dark, light);

    CFont f;
    f.CreateFont(Base::PointsToPixels(20), 0, 0, 0, FW_BOLD, 0, 0,
                 0, 0, 0, 0, 0, 0, "Verdana");
    CFont* pOldFont = dc.SelectObject(&f);

    GetDlgItem(IDC_CASHIER_GROUP4)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawRectAlpha(&dc, r, red, green, blue, alpha);
    Base::DrawBumpRect(&dc, r, dark, light);
    CStrOut balance;
    if (known_)
    {
      balance << "Balance: " << Balance_;
    }
    else
    {
      balance << "Balance: ...";
    }
    Base::DrawBumpText(&dc, balance.str(), r, DT_SINGLELINE|DT_CENTER|DT_VCENTER,
                       light, RGB(red, green, blue), dark);

    dc.SelectObject(pOldFont);

    dc.SelectObject(static_cast<CLoungeDlg*>(GetParent())->getFont());

    GetDlgItem(IDC_PROMPT_CREDITCARD)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Credit Card", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_BUYINAMOUNT)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Amount", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_CASHOUTAMOUNT)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Amount", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    GetDlgItem(IDC_PROMPT_ACTION)->GetWindowRect(&r);
    ScreenToClient(&r);
    Base::DrawBumpText(&dc, "Action", r.TopLeft(), RGB(255, 255, 255), RGB(0, 0, 0));

    dc.SetBkMode(oldMode);
  }
}

BOOL CCashierDlg::OnInitDialog() 
{
  if (Global::LoungeServer())
  {
    known_ = false;
    Balance_ = 0;
    Global::LoungeServer()->sendBalanceQueryPDU();
    Global::LoungeServer()->sendCardQueryPDU();
  }

  CDialog::OnInitDialog();

  BOOL isPlayMoney = CLoungeDlg::InPlayMoney();
  if (isPlayMoney)
  {
    EnableBuyinItems(FALSE);
    EnableCashoutItems(FALSE);
    GetDlgItem(IDC_CREDITCARDS)->EnableWindow(FALSE);
    SetWindowText("Play Money Cashier - querying...");
  }
  else
    SetWindowText("Real Money Cashier - querying...");

  return TRUE;  // return TRUE unless you set the focus to a control
}


void CCashierDlg::OnBuyin() 
{
  if (GetDlgItem(IDC_BUYINAMOUNT)->IsWindowEnabled())
  { // cancel buy-in
    GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
    EnableBuyinSubItems(FALSE);
    GetDlgItem(IDC_BUYIN)->SetWindowText("Buy In");
    GetDlgItem(IDC_CASHOUT)->EnableWindow(TRUE);
  }
  else
  { // allow buy-in
    EnableCashoutItems(FALSE);
    EnableBuyinSubItems(TRUE);
    GetDlgItem(IDC_BUYIN)->SetWindowText("Cancel");
    GetDlgItem(IDC_CREDITCARD)->SetFocus();
    UpdateCommitOnBuyin();
  }
}


void CCashierDlg::OnCashout() 
{
  if (GetDlgItem(IDC_CASHOUTAMOUNT)->IsWindowEnabled())
  { // cancel cash-out
    GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
    EnableCashoutSubItems(FALSE);
    GetDlgItem(IDC_CASHOUT)->SetWindowText("Cash Out");
    GetDlgItem(IDC_BUYIN)->EnableWindow(TRUE);
  }
  else
  { // allow cash out
    EnableBuyinItems(FALSE);
    EnableCashoutSubItems(TRUE);
    GetDlgItem(IDC_CASHOUT)->SetWindowText("Cancel");
    GetDlgItem(IDC_CASHOUTAMOUNT)->SetFocus();
    UpdateCommitOnCashout();
  }
}


void CCashierDlg::EnableCashoutItems(BOOL b)
{
  GetDlgItem(IDC_CASHOUT)->EnableWindow(b);
  GetDlgItem(IDC_CASHOUTAMOUNT)->EnableWindow(b);
}


void CCashierDlg::EnableBuyinSubItems(BOOL b)
{
  GetDlgItem(IDC_CREDITCARD)->EnableWindow(b);
  GetDlgItem(IDC_BUYINAMOUNT)->EnableWindow(b);
}


void CCashierDlg::EnableBuyinItems(BOOL b)
{
  GetDlgItem(IDC_BUYIN)->EnableWindow(b);
  GetDlgItem(IDC_CREDITCARD)->EnableWindow(b);
  GetDlgItem(IDC_BUYINAMOUNT)->EnableWindow(b);
}


void CCashierDlg::EnableCashoutSubItems(BOOL b)
{
  GetDlgItem(IDC_CASHOUTAMOUNT)->EnableWindow(b);
}


void CCashierDlg::OnChangeBuyinamount() 
{
  UpdateCommitOnBuyin();
}


void CCashierDlg::OnSelchangeCreditcard() 
{
  UpdateCommitOnBuyin();
}


void CCashierDlg::OnEditchangeCreditcard() 
{
  UpdateCommitOnBuyin();
}


void CCashierDlg::UpdateCommitOnBuyin()
{
  CString buyin_amount, credit_card;
  GetDlgItem(IDC_BUYINAMOUNT)->GetWindowText(buyin_amount);
  GetDlgItem(IDC_CREDITCARD)->GetWindowText(credit_card);

  if (!buyin_amount.IsEmpty() && !credit_card.IsEmpty())
  {
    GetDlgItem(IDC_COMMIT)->EnableWindow(TRUE);
    CString s2;
    s2.Format("Buy In $%s with %s", buyin_amount, credit_card);
    GetDlgItem(IDC_EDIT_COMMIT)->SetWindowText(s2);
  }
  else
  {
    GetDlgItem(IDC_EDIT_COMMIT)->SetWindowText("");
    GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
  }
}


void CCashierDlg::OnChangeCashoutamount() 
{
  UpdateCommitOnCashout();  	
}


void CCashierDlg::UpdateCommitOnCashout()
{
  CString cashout_amount;
  GetDlgItem(IDC_CASHOUTAMOUNT)->GetWindowText(cashout_amount);

  if (!cashout_amount.IsEmpty())
  {
    GetDlgItem(IDC_COMMIT)->EnableWindow(TRUE);
    CString s2;
    s2.Format("Cash Out $%s", cashout_amount);
    GetDlgItem(IDC_EDIT_COMMIT)->SetWindowText(s2);
  }
  else
  {
    GetDlgItem(IDC_EDIT_COMMIT)->SetWindowText("");
    GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
  }
}


void CCashierDlg::OnCommit() 
{
  bool showTransaction = false;
  CString s;
  GetDlgItem(IDC_EDIT_COMMIT)->SetWindowText(s);

  if (Global::LoungeServer())
  {
    USHORT card_index = 0;
    USHORT amount = 0;

    if (GetDlgItem(IDC_BUYIN)->IsWindowEnabled())
    { // we're doing a buyin
      GetDlgItem(IDC_BUYINAMOUNT)->GetWindowText(s);
      amount = atoi(s);

      card_index = creditCardCombo_.GetCurSel();
      if (card_index != CB_ERR)
      {
        CString text;
        creditCardCombo_.GetLBText(card_index, text);

        CString msg;
        msg.Format("Buy in $%d with %s?", amount, text);
        UINT action = AfxMessageBox(msg, MB_ICONQUESTION|MB_YESNO);

        if (action== IDYES)
        { 
          GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
          EnableBuyinSubItems(FALSE);
          GetDlgItem(IDC_BUYIN)->SetWindowText("Buy In");
          GetDlgItem(IDC_CASHOUT)->EnableWindow(TRUE);

          Global::LoungeServer()->sendBuyInRequestPDU(card_index, amount);

          showTransaction = true;
        }
        else
        {
          AfxMessageBox("Buy in not committed.", MB_OK|MB_ICONINFORMATION);
        }
      }
    }
    else if (GetDlgItem(IDC_CASHOUT)->IsWindowEnabled())
    { // we're doing a cashout
      GetDlgItem(IDC_CASHOUTAMOUNT)->GetWindowText(s);
      amount = atoi(s);

      CString msg;
      msg.Format("Cash out $%d?", amount);
      UINT action = AfxMessageBox(msg, MB_ICONQUESTION|MB_YESNO);

      if (action== IDYES)
      { 
        GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
        EnableCashoutSubItems(FALSE);
        GetDlgItem(IDC_CASHOUT)->SetWindowText("Cash Out");
        GetDlgItem(IDC_BUYIN)->EnableWindow(TRUE);

        Global::LoungeServer()->sendCashOutRequestPDU(amount);
        showTransaction = true;
      }
      else
      {
        AfxMessageBox("Cash out not committed.", MB_OK|MB_ICONINFORMATION);
      }
    }    
  }

  if (showTransaction && !pTransactionDlg_)
  {
    enableClose(FALSE);
    pTransactionDlg_ = new CTransactionDlg(this);
    pTransactionDlg_->Create(IDD_TRANSACTION, this);
    pTransactionDlg_->CenterWindow();
    pTransactionDlg_->ShowWindow(SW_SHOWNORMAL);
  }
}

void CCashierDlg::BuyInResponse(USHORT indicator)
{
  TransactionReply(NULL);

  CWnd* wnd = pDlg_;
  if (!wnd)
    wnd = AfxGetMainWnd();

  if (indicator == 1)
  {
    wnd->MessageBox("Buy in request accepted.", NULL, MB_ICONINFORMATION);
  }
  else
  {
    wnd->MessageBox("Buy in request was denied.", NULL, MB_ICONINFORMATION);
  }
}

void CCashierDlg::CashOutResponse(USHORT indicator)
{
  TransactionReply(NULL);

  CWnd* wnd = pDlg_;
  if (!wnd)
    wnd = AfxGetMainWnd();

  if (indicator == 1)
  {
    wnd->MessageBox("Cash out request accepted.", NULL, MB_ICONINFORMATION);
  }
  else
  {
    wnd->MessageBox("Cash out request was denied.", NULL, MB_ICONINFORMATION);
  }
}

void CCashierDlg::TransactionReply(const char* msg)
{
  if (pDlg_)
  {
    pDlg_->transactionReply(msg);
    if (Global::LoungeServer())
      Global::LoungeServer()->sendBalanceQueryPDU();
  }

  if (msg != NULL && *msg != '\0')
    pDlg_->MessageBox(msg, NULL, MB_OK|MB_ICONINFORMATION);
}

void CCashierDlg::transactionReply(const char* msg)
{
  if (pTransactionDlg_)
  {
    enableClose(TRUE);
    pTransactionDlg_->DestroyWindow();
    delete pTransactionDlg_;
    pTransactionDlg_ = 0;
  }
}

void CCashierDlg::SetBalance(const CChips& balance)
{
  if (!known_ || Balance_ != balance)
  {
    Balance_ = balance;
    if (pDlg_)
    {
      CRect r;
      pDlg_->GetDlgItem(IDC_CASHIER_GROUP4)->GetWindowRect(&r);
      pDlg_->ScreenToClient(&r);
      pDlg_->InvalidateRect(&r);
    }
  }
  known_ = true;
}

void CCashierDlg::AddCreditCards(int num_cards,
                                 PDUCreditCardReply::CardInfo* pCI)
{
  CWnd* pComboBox = 0;
  if (pDlg_)
    pComboBox = pDlg_->GetDlgItem(IDC_CREDITCARD);
  
  if (pComboBox)
  {
    for (int i = 0; i < num_cards; i++)
    {
      CString s;
      s.Format("XXXX-%c%c%c%c %s", pCI->last_4_digits_[0],
                                   pCI->last_4_digits_[1],
                                   pCI->last_4_digits_[2],
                                   pCI->last_4_digits_[3],
                                   pCI->card_type_);
      pCI++;

      pComboBox->SendMessage(CB_ADDSTRING, 0, (LPARAM)(LPCSTR)s);
    }

    if (num_cards > 0)
      pComboBox->SendMessage(CB_SETCURSEL, MAKEWPARAM(0, 0), 0);
  }

  if (pDlg_)
  {
    if (CLoungeDlg::InPlayMoney())
      pDlg_->SetWindowText("Play Money Cashier");
    else
      pDlg_->SetWindowText("Real Money Cashier");
  }
}


/*
void CCashierDlg::OnUpdateBuyin(CCmdUI* pCmdUI)
{
    // allow buyin if credit cards
    pCmdUI->Enable(creditCardCombo_.GetCount() > 0);
}

void CCashierDlg::OnUpdateCashout(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(Balance_ > 0);
}
*/

BOOL CCashierDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    BOOL rc = FALSE;

    UINT cmd = LOWORD(wParam);
    if (cmd == IDC_PROGRESS1)
    {
      rc = TRUE;

      // Protect against reentry caused by fast wm_timer
      // in transactiondialog
      static BOOL inHere = FALSE;
      if (inHere) return TRUE;
      inHere = TRUE;

      if (pTransactionDlg_)
      {
        AfxMessageBox("Transaction timed out!");
        pTransactionDlg_->DestroyWindow();
        delete pTransactionDlg_;
        pTransactionDlg_ = 0;
        enableClose(TRUE);
      }

      inHere = FALSE;
    }
	
    if (!rc)
	  rc = CDialog::OnCommand(wParam, lParam);

    return rc;
}

void CCashierDlg::enableClose(BOOL b)
{
  // Enable/disable whole window
  EnableWindow(b);
  GetDlgItem(IDOK)->EnableWindow(b);
}

BOOL IsButton(int ctrlID)
{
    return ctrlID == IDC_BUYIN ||
           ctrlID == IDC_CASHOUT ||
           ctrlID == IDC_COMMIT ||
           ctrlID == IDOK;
}

BOOL CCashierDlg::PreTranslateMessage(MSG* pMsg) 
{
  BOOL rc = FALSE;

  // If focus is in some other control than a button,
  // eat ENTER so the app won't exit accidentally
  if (pMsg->message == WM_KEYDOWN &&
      pMsg->wParam == VK_RETURN)
  {
    CWnd* pWnd = GetFocus();
    if (pWnd)
    {
      int ctrlID = pWnd->GetDlgCtrlID();
      if (!IsButton(ctrlID))
        return TRUE; // Ignore
    }
  }
	
  return CDialog::PreTranslateMessage(pMsg);
}


void CCashierDlg::OnCreditcards() 
{
  CCreditCardDlg dlg;
  if (dlg.DoModal() == IDOK)
  {
    if (Global::LoungeServer())
    {
      Global::LoungeServer()->sendCardQueryPDU();
      SetWindowText("Real Money Cashier - querying...");

      CWnd* pComboBox = GetDlgItem(IDC_CREDITCARD);
      ((CComboBox*)pComboBox)->ResetContent();
      // Disable BUYIN items
      GetDlgItem(IDC_COMMIT)->EnableWindow(FALSE);
      EnableBuyinSubItems(FALSE);
      GetDlgItem(IDC_BUYIN)->SetWindowText("Buy In");
      GetDlgItem(IDC_CASHOUT)->EnableWindow(TRUE);
    }
  }
}
