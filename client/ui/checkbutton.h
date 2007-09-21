#ifndef __psi_checkbutton_h__
#define __psi_checkbutton_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CCheckButton
  Created  : 19.12.1999

  OVERVIEW : CCheckButton implements an owner-drawn check one button.

 ****************************************************************************/


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "base/transparentbitmap.h"


class CCheckButton : public CButton
{
// Construction
public:
  CCheckButton();

  BOOL isChecked() const { return isChecked_; }
  void setChecked(BOOL b) { isChecked_ = b; }

  static void MakeCheckmarkBitmap();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CActionButton)
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  //}}AFX_VIRTUAL

protected:
  //{{AFX_MSG(CCheckButton)
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  static Base::TransparentBitmap bmpCheck_;
  UINT  isChecked_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
