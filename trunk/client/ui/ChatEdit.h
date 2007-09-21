#ifndef __psi_chatedit_h__
#define __psi_chatedit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatEdit
  Created  : 25.10.1999

  OVERVIEW : CChatEdit implements the edit box in CChatView.

 ****************************************************************************/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CChatEdit window

class CChatEdit : public CEdit
{
  DECLARE_DYNAMIC(CChatEdit)

// Construction
public:
	CChatEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChatEdit();

	// Generated message map functions
protected:
  //{{AFX_MSG(CChatEdit)
    // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
