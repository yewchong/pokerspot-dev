#ifndef __psi_pokerapp_h__
#define __psi_pokerapp_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CPokerApp
  Created  : 25.10.1999

  OVERVIEW : CPokerApp is the CWinApp derivative class.

 ****************************************************************************/


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ui/resource.h"

class CPokerApp : public CWinApp
{
public:
  CPokerApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPokerApp)
public:
  virtual BOOL InitInstance();
  virtual BOOL OnIdle(LONG lCount);
  virtual int Run();
  virtual int ExitInstance();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

// Implementation

  //{{AFX_MSG(CPokerApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
