#ifndef __psi_waitdlg_h__
#define __psi_waitdlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CWaitDlg
  Created  : 28.10.1999

  OVERVIEW : Dialog for prompting the user to wait until
             new hand start.

 ****************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog

class CWaitDlg : public CDialog
{
// Construction
public:
  CWaitDlg(CWnd* pParent = NULL);   // standard constructor

  void setPrompt(LPCTSTR pszPrompt);

// Dialog Data
	//{{AFX_DATA(CWaitDlg)
	enum { IDD = IDD_WAITING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
