#ifndef __psi_cashierdlg_h__
#define __psi_cashierdlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "loungedlg/resource.h"
#include "base/jpgimage.h"
#include "network/pducreditcardresponse.h"

using Network::PDUCreditCardReply;
class CTransactionDlg;
class CChips;

/////////////////////////////////////////////////////////////////////////////
// CCashierDlg dialog

class CCashierDlg : public CDialog
{
public:
  static void AddCreditCards(int num_cards,
                             PDUCreditCardReply::CardInfo*);

  static void BuyInResponse(USHORT indicator);
  static void CashOutResponse(USHORT indicator);
  static void SetBalance(const CChips&);
  static void TransactionReply(const char* msg);

public:
  CCashierDlg(CWnd* pParent = NULL);
  ~CCashierDlg();

  //{{AFX_DATA(CCashierDlg)
  enum { IDD = IDD_CASHIER };
  CComboBox	creditCardCombo_;
  //}}AFX_DATA


  //{{AFX_VIRTUAL(CCashierDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:

  //{{AFX_MSG(CCashierDlg)
  afx_msg void OnPaint();
  virtual BOOL OnInitDialog();
  afx_msg void OnBuyin();
  afx_msg void OnCashout();
  afx_msg void OnChangeBuyinamount();
  afx_msg void OnSelchangeCreditcard();
  afx_msg void OnChangeCashoutamount();
  afx_msg void OnCommit();
  afx_msg void OnEditchangeCreditcard();
  afx_msg void OnCreditcards();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  Base::JPGImage* pImage_;

  CTransactionDlg* pTransactionDlg_;

  void EnableCashoutItems(BOOL);
  void EnableBuyinSubItems(BOOL);
  void EnableBuyinItems(BOOL);
  void EnableCashoutSubItems(BOOL);
  void UpdateCommitOnBuyin();
  void UpdateCommitOnCashout();
  void enableClose(BOOL);
  void transactionReply(const char* msg);

  static CCashierDlg* pDlg_;
  static CChips Balance_;
  static bool   known_;
};

//{{AFX_INSERT_LOCATION}}

#endif 
