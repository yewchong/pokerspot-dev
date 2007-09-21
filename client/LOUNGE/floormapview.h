#ifndef __psi_floormapview_h__
#define __psi_floormapview_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CFloorMapView
  Created  : 25.10.1999

  OVERVIEW : Implements the floor map view.

 ****************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

namespace Base { class Table; }
using Base::Table;

class CLoungeDlg2;

/////////////////////////////////////////////////////////////////////////////
// CFloorMapView view

class CFloorMapView : public CView
{
public:
  DECLARE_DYNAMIC(CFloorMapView);

  CFloorMapView();  

// Operations
public:
  void addTable(int number, int players, int lolimit, int hilimit);
  int  numTables() const { return tables_.size(); }
  void removeTables();
  void recalcLayout();
  void setTitle();
  void getTableLimits(USHORT number, USHORT& lolimit, USHORT& hilimit);

  static CFloorMapView* Instance() { return pInstance_; }

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFloorMapView)
public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual void OnInitialUpdate();
protected:
  virtual void OnDraw(CDC* pDC);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CFloorMapView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CFloorMapView)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnFakePdustatus();
  afx_msg void OnUpdateFakePdustatus(CCmdUI* pCmdUI);
  afx_msg void OnFakePdusitting();
  afx_msg void OnUpdateFakePdusitting(CCmdUI* pCmdUI);
  afx_msg void OnFakePduloungeaccept();
  afx_msg void OnUpdateFakePduloungeaccept(CCmdUI* pCmdUI);
  afx_msg void OnFakePdulogoutwarning();
  afx_msg void OnUpdateFakePdulogoutwarning(CCmdUI* pCmdUI);
  afx_msg void OnJoinTable();
  afx_msg void OnUpdateJoinTable(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CBrush brBackground_;
  CBitmap bmpBackground_;
  int width_, height_;

  typedef std::list<Table*> Tables;

  Tables tables_, selection_;
  Table* hitTest(const CPoint& pt);
  void setSelection(Table*);
  void modifySelection(Table*);
  void removeFromSelection(Table*);
  void drawSelection(CDC*);
  void invalidateSelection();
  bool isSelected(Table*) const;
  void updateButtons();

  static CFloorMapView* pInstance_;

  CLoungeDlg2* pLoungeDlg_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
