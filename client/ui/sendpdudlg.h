#if !defined(AFX_SENDPDUDLG_H__1BA8B981_B270_11D3_BDBC_0080C750238F__INCLUDED_)
#define AFX_SENDPDUDLG_H__1BA8B981_B270_11D3_BDBC_0080C750238F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// sendpdudlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendPDUDlg dialog

class CSendPDUDlg : public CDialog
{
// Construction
public:
	CSendPDUDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendPDUDlg)
	enum { IDD = IDD_PDU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendPDUDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendPDUDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDPDUDLG_H__1BA8B981_B270_11D3_BDBC_0080C750238F__INCLUDED_)
