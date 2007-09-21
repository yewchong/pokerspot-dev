#ifndef __psi_chatframe_h__
#define __psi_chatframe_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatFrame
  Created  : 5.11.1999

  OVERVIEW : CChatFrame is the CFrameWnd used for CChatView.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CChatFrame frame

class CChatFrame : public CFrameWnd
{
public:
  CChatFrame();


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CChatFrame();

	// Generated message map functions
	//{{AFX_MSG(CChatFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
