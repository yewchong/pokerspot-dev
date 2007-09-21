#if !defined(AFX_COMMENTDIALOG_H__6624DC44_671A_11D5_856E_C08760C10801__INCLUDED_)
#define AFX_COMMENTDIALOG_H__6624DC44_671A_11D5_856E_C08760C10801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// commentdialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommentDialog dialog

class CCommentDialog : public CDialog
{
// Construction
public:
	CCommentDialog(const CString& username, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCommentDialog)
	enum { IDD = IDD_EDITCOMMENT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommentDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommentDialog)
	virtual BOOL OnInitDialog();
    virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  CString username_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMENTDIALOG_H__6624DC44_671A_11D5_856E_C08760C10801__INCLUDED_)
