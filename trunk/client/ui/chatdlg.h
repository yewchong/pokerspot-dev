#ifndef __psi_chatdlg_h__
#define __psi_chatdlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CChatDlg
  Created  : 5.11.1999

  OVERVIEW : Dialog class containing the CChatView.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChatFrame;

#include "ui/resource.h"


/////////////////////////////////////////////////////////////////////////////
// CChatDlg dialog

class CChatDlg : public CDialog
{
// Construction
public:
  CChatDlg(CWnd* pParent = NULL);   // standard constructor
  ~CChatDlg();

// Dialog Data
  //{{AFX_DATA(CChatDlg)
  enum { IDD = IDD_CHAT };
  CTabCtrl tab_;
  //}}AFX_DATA

  // Docking
  enum ChatDlg_DockMode
  {
    CD_NoDock    = 0,
    CD_DockRight = 1,
    CD_DockLeft  = 2
  };
  void dock(ChatDlg_DockMode);
  ChatDlg_DockMode isDocked() const;
  void attach();      
  int getMinWidth() const;

  // Lounge has no tabs, allow hiding
  void hideTabs();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CChatDlg)
public:
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
  virtual BOOL OnInitDialog();
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CChatDlg)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg UINT OnNcHitTest(CPoint point);
  afx_msg void OnRadioAll();
  afx_msg void OnRadioDealer();
  afx_msg void OnRadioUser();
  afx_msg void OnCheckSystemOnly();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  bool checkSendTime();

private:
  CChatFrame* pChatFrame_;
  DWORD       lastMsgTime_;
  ChatDlg_DockMode docking_;
};

#define DefaultChatWndDock 1

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
