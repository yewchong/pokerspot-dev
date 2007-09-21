#ifndef __psi_promptinputdlg_h__
#define __psi_promptinputdlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ui/resource.h"

class CPromptInputDlg : public CDialog
{
// Construction
public:
  CPromptInputDlg(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPromptInputDlg)
	enum { IDD = IDD_PROMPTINPUT };
	CString	input_;
	CString	prompt_;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPromptInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPromptInputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
