#pragma once
#ifndef __psi_aboutdlg_h__
#define __psi_aboutdlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CAboutDlg
  Created  : 25.10.1999

  OVERVIEW : About dialog.

 ****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PSPOT_TABLE_CLIENT_
#include "ui/resource.h"
#else
#include "loungedlg/resource.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  //{{AFX_DATA(CAboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlg)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CBrush  brBackground_;
  CBitmap bmpLogo_;
  CFont   font_;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
