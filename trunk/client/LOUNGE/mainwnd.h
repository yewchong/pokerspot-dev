#ifndef __psi_mainwnd_h__
#define __psi_mainwnd_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMainWnd
  Created  : 25.10.1999

  OVERVIEW : CMainWnd is the main window class. It is
             derived from CFrameWnd.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ui/chatdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainWnd frame

class CMainWnd : public CFrameWnd
{
public:
  enum
  {
    MW_Maximized = -2
  };

public:
  CMainWnd();

// Attributes
public:

// Operations
public:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainWnd)
public:
  virtual BOOL DestroyWindow();
protected:
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  //}}AFX_VIRTUAL

public:
  void saveSettings();
  void restoreSettings();

  static CMainWnd* Instance() { return pInstance_; }
  static void SetStatusText(const CString&);

// Implementation
protected:
  virtual ~CMainWnd();

  // Generated message map functions
  //{{AFX_MSG(CMainWnd)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnClose();
  afx_msg void OnAppAbout();
  afx_msg void OnLogin();
  afx_msg void OnUpdateLogin(CCmdUI* pCmdUI);
  afx_msg void OnLogout();
  afx_msg void OnUpdateLogout(CCmdUI* pCmdUI);
  afx_msg void OnMotd();
  afx_msg void OnFloormap();
  afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
  afx_msg void OnSoundtoggle();
  afx_msg void OnUpdateSoundtoggle(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOnlineIndicator(CCmdUI* pCmdUI);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  // This splitter splits the main window
  // into game and chat panes.
  // NOT USED any more!
  CSplitterWnd splitterWnd_;

  // This is the chat window
  CChatDlg   chatDlg_;

  CStatusBar statusBar_;

  static CMainWnd* pInstance_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
