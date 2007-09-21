#if !defined(AFX_EDITORDLG_H__1ED923A0_7A7B_11D4_9D99_00004B305686__INCLUDED_)
#define AFX_EDITORDLG_H__1ED923A0_7A7B_11D4_9D99_00004B305686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// editordlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg dialog

class CEditorDlg : public CDialog
{
// Construction
public:
	CEditorDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CEditorDlg();

// Dialog Data
	//{{AFX_DATA(CEditorDlg)
	enum { IDD = IDD_EDITOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
    static CEditorDlg* Inst();
    void setText(const char*);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    static CEditorDlg* pInst_;
    int xofs_, yofs_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORDLG_H__1ED923A0_7A7B_11D4_9D99_00004B305686__INCLUDED_)
