#ifndef __psi_logindlg_h__
#define __psi_logindlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CLoginDlg
  Created  : 27.10.1999

  OVERVIEW : Login dialog class.

 ****************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
  CLoginDlg(CWnd* pParent = NULL);   // standard constructor
  ~CLoginDlg();  

// Dialog Data
  //{{AFX_DATA(CLoginDlg)
  CString host_;
  int     port_;
  CString password_;
  CString username_;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLoginDlg)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CLoginDlg)
  afx_msg void OnChangePassword();
  afx_msg void OnChangeUsername();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  void enableOK();
  BOOL savePassword() const;

public:
  static ULONG  IpAddress_;
  static USHORT Port_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
