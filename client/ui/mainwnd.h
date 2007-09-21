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

#include "actionbar.h"
#include "chatdlg.h"

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
  CMainWnd(UINT floorMapMessage);

// Attributes
public:

// Operations
public:
  BOOL Create(BOOL show_splash, LPCTSTR lpszClassName, LPCTSTR lpszWindowName);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMainWnd)
public:
  virtual BOOL DestroyWindow();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

public:
  void saveSettings();
  void restoreSettings();

  static CMainWnd* Instance() { return pInstance_; }
  static void SetStatusText(const CString&);
  int    getStatusBarHeight() const;

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
  afx_msg void OnTable();
  afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
  afx_msg void OnSoundtoggle();
  afx_msg void OnUpdateSoundtoggle(CCmdUI* pCmdUI);
  afx_msg void onTestDownload();
  afx_msg void OnOptions();
  afx_msg void OnRequest5handlog();
  afx_msg void OnUpdateRequest5handlog(CCmdUI* pCmdUI);
  afx_msg void OnRequestprevhand();
  afx_msg void OnUpdateRequestprevhand(CCmdUI* pCmdUI);
  afx_msg void OnMove(int x, int y);
  afx_msg void OnBuyin();
  afx_msg void OnUpdateBuyin(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnTestloginout();
  afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  //}}AFX_MSG
  afx_msg void OnUpdateOnlineIndicator(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()

private:
  // This splitter splits the main window
  // into game and chat panes.
  // NOT USED any more!
  CSplitterWnd splitterWnd_;

  // This bar contains buttons for user actions.
  CActionBar actionBar_;
  void recreateActionBar();
  void toggleActionBar(UINT cmd);
  void toggleChatWnd(int mode);

  // This is the chat window
  CChatDlg   chatDlg_;

  CStatusBar statusBar_;

  static CMainWnd* pInstance_;

  UINT floorMapMessage_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
