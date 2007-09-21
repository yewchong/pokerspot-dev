#ifndef __psi_tabctrl_h__
#define __psi_tabctrl_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class TabCtrl
  Created  : 02/03/2000

  OVERVIEW : TabCtrl is an owner-draw subclass of CTabCtrl.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TabCtrl : public CTabCtrl
{
public:
  TabCtrl();

  //{{AFX_MSG(TabCtrl)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()


  //{{AFX_VIRTUAL(TabCtrl)
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  //}}AFX_VIRTUAL
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 

