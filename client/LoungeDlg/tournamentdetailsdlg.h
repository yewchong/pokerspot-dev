#if !defined(AFX_TOURNAMENTDETAILSDLG_H__0DC80D40_53FE_11D4_9D99_00004B305686__INCLUDED_)
#define AFX_TOURNAMENTDETAILSDLG_H__0DC80D40_53FE_11D4_9D99_00004B305686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tournamentdetailsdlg.h : header file
//

#include "loungedlg/resource.h"

class CChips;

/////////////////////////////////////////////////////////////////////////////
// CTournamentDetailsDlg dialog

class CTournamentDetailsDlg : public CDialog
{
// Construction
public:
    CTournamentDetailsDlg(CWnd* pParent = NULL);   // standard constructor

    void setQueryingTitle(USHORT tournament);
    void setData(const CChips& buyin,
                 ULONG starttime,
                 USHORT numplayers,
                 const char* descr);
    void setFinishedTitle();

// Dialog Data
	//{{AFX_DATA(CTournamentDetailsDlg)
	enum { IDD = IDD_TOURNAMENTDETAILS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTournamentDetailsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTournamentDetailsDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    int tournament_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOURNAMENTDETAILSDLG_H__0DC80D40_53FE_11D4_9D99_00004B305686__INCLUDED_)
