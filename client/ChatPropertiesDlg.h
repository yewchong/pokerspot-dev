#if !defined(AFX_CHATPROPERTIESDLG_H__440DF080_E4C9_11D3_9D99_00004B305686__INCLUDED_)
#define AFX_CHATPROPERTIESDLG_H__440DF080_E4C9_11D3_9D99_00004B305686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChatPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChatPropertiesDlg dialog

class CChatPropertiesDlg : public CDialog
{
// Construction
public:
	CChatPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChatPropertiesDlg)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChatPropertiesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATPROPERTIESDLG_H__440DF080_E4C9_11D3_9D99_00004B305686__INCLUDED_)
