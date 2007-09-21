#ifndef __psi_motddlg_h__
#define __psi_motddlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdDlg
  Created  : 25.10.1999

  OVERVIEW : Message of the day dialog.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMotdFrame;

#include "ui/resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMotdDlg dialog

class CMotdDlg : public CDialog
{
// Construction
public:
  CMotdDlg(LPCTSTR pszMotdMessage, LPCTSTR pszCaption, CWnd* pParent = NULL);   // standard constructor
  ~CMotdDlg();

// Dialog Data
	//{{AFX_DATA(CMotdDlg)
	enum { IDD = IDD_MOTD };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotdDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMotdDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  CMotdFrame* pHTMLFrame_;
  CString     caption_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
