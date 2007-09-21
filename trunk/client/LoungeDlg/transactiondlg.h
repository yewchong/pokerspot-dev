#if !defined(AFX_TRANSACTIONDLG_H__9CDC4360_2B3C_11D4_856E_608953C10001__INCLUDED_)
#define AFX_TRANSACTIONDLG_H__9CDC4360_2B3C_11D4_856E_608953C10001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// transactiondlg.h : header file
//

class CProgressIndicator;

/////////////////////////////////////////////////////////////////////////////
// CTransactionDlg dialog

class CTransactionDlg : public CDialog
{
// Construction
public:
	CTransactionDlg(CWnd* pParent = NULL);   // standard constructor
    ~CTransactionDlg();

// Dialog Data
	//{{AFX_DATA(CTransactionDlg)
	enum { IDD = IDD_TRANSACTION };
	//}}AFX_DATA

    CProgressIndicator* progressInd_;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransactionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    UINT timer_;
    long lastTime_;
    int  queriesSent_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIONDLG_H__9CDC4360_2B3C_11D4_856E_608953C10001__INCLUDED_)
