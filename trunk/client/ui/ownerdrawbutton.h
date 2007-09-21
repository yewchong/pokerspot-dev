#ifndef __psi_ownerdrawbutton_h__
#define __psi_ownerdrawbutton_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class COwnerDrawButton
  Created  : 01/31/2000

  OVERVIEW : COwnerDrawButton for the lounge client.

 ****************************************************************************/


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



class COwnerDrawButton : public CButton
{
// Construction
public:
  COwnerDrawButton();

// Attributes
public:

// Operations
public:
  void setHighLight(BOOL b) { hilite_ = b; }
  BOOL getHighLight() const { return hilite_; }


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COwnerDrawButton)
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~COwnerDrawButton();

  // Generated message map functions
protected:
  //{{AFX_MSG(COwnerDrawButton)
  afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()

private:
  BOOL hilite_;
  static CBrush brHilite_;
  static CFont  fontHilite_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
