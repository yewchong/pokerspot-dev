#ifndef __psi_actionbutton_h__
#define __psi_actionbutton_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CActionButton
  Created  : 25.10.1999

  OVERVIEW : CActionButton implements one button in the action bar.
             TODO: implement an animating button.

 ****************************************************************************/


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "base/transparentbitmap.h"


class CActionButton : public CButton
{
// Construction
public:
  CActionButton();

// Attributes
public:

// Operations
public:
  void setHighLight(BOOL b) { hilite_ = b; }
  BOOL getHighLight() const { return hilite_; }


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CActionButton)
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CActionButton();

  // Generated message map functions
protected:
  //{{AFX_MSG(CActionButton)
  afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()

private:
  BOOL hilite_;
  static CBrush brHilite_;
  static CFont  fontHilite_;
  static Base::TransparentBitmap bmp_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
