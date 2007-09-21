#ifndef __psi_listctrl_h__
#define __psi_listctrl_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class ListCtrl
  Created  : 02/03/2000

  OVERVIEW : ListCtrl is an owner-draw subclass of CListCtrl.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class ListCtrl : public CListCtrl
{
public:
  BOOL getColumnWidth(int col, int& cx);
  BOOL getHeaderHeight(int& cy);
  BOOL getItemText(int row, int col, CString&);
  BOOL setItemText(int row, int col, const CString&);

  void push(int col, const CString& s);
  CRect hilite(int row, int col);
  void hilite(int col, const CString& s);
  void remove(int col, const CString& s);

  //{{AFX_VIRTUAL(ListCtrl)
public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  //}}AFX_VIRTUAL
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
