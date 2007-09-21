#ifndef __psi_lounge_h__
#define __psi_lounge_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"


class CLoungeApp : public CWinApp
{
public:
  CLoungeApp();

  void saveSettings();
  void restoreSettings();

  //{{AFX_VIRTUAL(CLoungeApp)
	public:
  virtual BOOL InitInstance();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

  //{{AFX_MSG(CLoungeApp)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}

#endif 
