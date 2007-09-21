#if !defined(AFX_INPUTDLG_H__7750DBC4_9EA4_11D4_856E_30A660C10801__INCLUDED_)
#define AFX_INPUTDLG_H__7750DBC4_9EA4_11D4_856E_30A660C10801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// inputdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog

class CInputDlg : public CDialog
{
// Construction
public:
	CInputDlg(CWnd* pParent = NULL);   // standard constructor

    CString getText() const;

// Dialog Data
	//{{AFX_DATA(CInputDlg)
	enum { IDD = IDD_FLOORMANMSG };
	CString	text_;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDLG_H__7750DBC4_9EA4_11D4_856E_30A660C10801__INCLUDED_)
