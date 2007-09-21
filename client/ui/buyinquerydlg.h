#ifndef __psi_buyinquerydlg_h__
#define __psi_buyinquerydlg_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CBuyInQueryDlg
  Created  : 11.12.1999

  OVERVIEW : Buy In Query dialog class.

 ****************************************************************************/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ui/resource.h"
#include "common/chips.h"

class CBuyInQueryDlg : public CDialog
{
// Construction
public:
  CBuyInQueryDlg(const CChips& limit, CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CBuyInQueryDlg)
	enum { IDD = IDD_BUYINQUERY };
	double chips_;
	CString	prompt_;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuyInQueryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBuyInQueryDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  CChips limit_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif 
