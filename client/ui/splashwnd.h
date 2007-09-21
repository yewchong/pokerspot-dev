#ifndef __psi_splashwnd_h__
#define __psi_splashwnd_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSplashWnd
  Created  : 03/10/2000

  OVERVIEW : Implements a splash screen window.

 ****************************************************************************/

namespace Base { class JPGImage; }

class CSplashWnd : public CWnd
{
protected:
	CSplashWnd();

public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static BOOL ShowSplashScreen(LPCTSTR imagefile,
                                 BOOL postLogin = TRUE,
                                 BOOL quitOnTimer = TRUE,
                                 CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

public:
	~CSplashWnd();
	virtual void PostNcDestroy();

        static CSplashWnd* c_pSplashWnd;

protected:
	BOOL Create(LPCTSTR,
                BOOL postLogin,
                BOOL quitOnTimer,
                CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;

protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  Base::JPGImage* pImage_;
  BOOL postLogin_; // post login message at exit
  BOOL quitOnTimer_;
};


#endif
