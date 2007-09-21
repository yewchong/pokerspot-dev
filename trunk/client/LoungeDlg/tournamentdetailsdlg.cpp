// tournamentdetailsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lounge.h"
#include "tournamentdetailsdlg.h"
#include "editordlg.h"
#include "ui/global.h"
#include "network/server.h"
#include "common/chips.h"
#include "common/stringoutput.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTournamentDetailsDlg dialog


CTournamentDetailsDlg::CTournamentDetailsDlg(CWnd* pParent /*=NULL*/)
	:
    CDialog(CTournamentDetailsDlg::IDD, pParent),
    tournament_(0)
{
	//{{AFX_DATA_INIT(CTournamentDetailsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTournamentDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTournamentDetailsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTournamentDetailsDlg, CDialog)
	//{{AFX_MSG_MAP(CTournamentDetailsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTournamentDetailsDlg message handlers

void CTournamentDetailsDlg::setQueryingTitle(USHORT tournament)
{
  tournament_ = tournament;
  CString s;
  s.Format("Tournament Details - querying...", tournament);
  SetWindowText(s);
}


void CTournamentDetailsDlg::setFinishedTitle()
{
  CString s;
  s.Format("Tournament Details", tournament_);
  SetWindowText(s);
}


void CTournamentDetailsDlg::setData(const CChips& buyin,
                                    ULONG starttime,
                                    USHORT numplayers,
                                    const char* descr)
{
    CStrOut s;
    if (buyin == 0)
        s << "Freeroll ($0)";
    else
        s << buyin;
    GetDlgItem(IDC_TD_BUYIN)->SetWindowText(s.str());

    time_t t = starttime;
    time_t t2 = time(NULL);

    CString str;
    str.Format("%s", ctime(&t));
    if (str.Find("\n") != -1)
    {
        str = str.Left(str.Find("\n"));
    }

    // If the start time is small, it indicates that this
    // is a satellite tournament and the number is the
    // number of players in the waiting list required for
    // the tournament to start

    CTime t3(2000, 1, 1, 00, 00, 0 ); // 00:00:00 Jan 1, 2000
    if (starttime < t3.GetTime())
    {
      str.Format("When %d players in waiting list.", starttime);
    }

    GetDlgItem(IDC_TD_STARTTIME)->SetWindowText(str);

    str.Format("%d", numplayers);
    GetDlgItem(IDC_TD_PLAYERS)->SetWindowText(str);

    GetDlgItem(IDC_TD_DESCRIPTION)->SetWindowText(descr);

    setFinishedTitle();
}

