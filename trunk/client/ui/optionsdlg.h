#ifndef __psi_optionsdlg_h__
#define __psi_optionsdlg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ui/resource.h"

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	afx_msg void OnCheckSettingsHorzactionbar();
	afx_msg void OnCheckSettingsSounds();
	afx_msg void OnCheckSettinsTtips();
	afx_msg void OnCheckSettingsTakefocus();
	afx_msg void OnSettingsDockactionbar();
	afx_msg void OnSettingsDockchatwnd();
	afx_msg void OnSettingsDockchatleft();
	//}}AFX_MSG
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR,
                                 LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif 
