#ifndef __psi_motdframe_h__
#define __psi_motdframe_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdFrame
  Created  : 28.10.1999

  OVERVIEW : CFrameWnd derivative for the MOTD dialog.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMotdFrame frame

class CMotdFrame : public CFrameWnd
{
public:
	CMotdFrame(LPCTSTR pszURL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotdFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMotdFrame();

	// Generated message map functions
	//{{AFX_MSG(CMotdFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  CStatusBar statusBar_;
  CString    motdMessage_;

  CString    saveToTmpFile(const CString&);
  CString    tmpFileName_;

  bool isURL(const CString&) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
