#ifndef __psi_motdview_h__
#define __psi_motdview_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdView
  Created  : 28.10.1999

  OVERVIEW : CHtmlView derivative view class for the MOTD dialog.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CMotdView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

// Will use CHtmlView
//#define HTML_VIEW_

#ifdef HTML_VIEW_
  #include <afxhtml.h>
#else
  #define CHtmlView CView
#endif

class CMotdView : public CHtmlView
{
public:
	CMotdView();           // protected constructor used by dynamic creation

// html Data
public:
	//{{AFX_DATA(CMotdView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotdView)
	public:
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnNavigateComplete2(LPCTSTR strURL);
        virtual void OnDraw(CDC*);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMotdView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMotdView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
