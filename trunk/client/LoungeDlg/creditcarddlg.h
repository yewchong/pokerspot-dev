#ifndef __psi_creditcarddlg_h__
#define __psi_creditcarddlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// creditcarddlg.h : header file
//

#include "network/pducreditcardreply2.h"
using Network::PDUCreditCardReply2;
#include <vector>
#include <list>
#include <string>


/////////////////////////////////////////////////////////////////////////////
// CCreditCardDlg dialog

class CCreditCardDlg : public CDialog
{
public:
	CCreditCardDlg(CWnd* pParent = NULL);
    ~CCreditCardDlg();

// Dialog Data
	//{{AFX_DATA(CCreditCardDlg)
	enum { IDD = IDD_CREDITCARDS };
	CString	_cc1_number_;
	CString	_cc2_number_;
	CString	_cc3_number_;
	CString	_cc4_number_;
	//}}AFX_DATA

  static void AddCreditCards(int num_cards,
                             PDUCreditCardReply2::CardInfo*);

  static void AddCreditCardTypes(const std::list<std::string>& cctypes);
  static u_int16_t GetCCTypeIndex(const CString& typeName);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreditCardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreditCardDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void enableControls(bool);

private:
  static CCreditCardDlg* pDlg_;
  static std::vector<std::string> cctypes_;

  CString cc1_type_, cc1_number_, cc1_expmo_, cc1_expyr_,
          cc2_type_, cc2_number_, cc2_expmo_, cc2_expyr_,
          cc3_type_, cc3_number_, cc3_expmo_, cc3_expyr_,
          cc4_type_, cc4_number_, cc4_expmo_, cc4_expyr_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
