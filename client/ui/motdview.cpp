/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMotdView
  Created  : 28.10.1999

  OVERVIEW : CHtmlView derivative view class for the MOTD dialog.

 ****************************************************************************/

#include "stdafx.h"
#include "motdview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMotdView


CMotdView::CMotdView()
{
	//{{AFX_DATA_INIT(CMotdView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMotdView::~CMotdView()
{
}

void CMotdView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMotdView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMotdView, CHtmlView)
	//{{AFX_MSG_MAP(CMotdView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMotdView diagnostics

#ifdef _DEBUG
void CMotdView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMotdView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMotdView message handlers

void CMotdView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel) 
{
#ifdef HTML_VIEW_
  CHtmlView::OnBeforeNavigate2(lpszURL, nFlags,	lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
#endif
}

void CMotdView::OnDocumentComplete(LPCTSTR lpszURL) 
{
#ifdef HTML_VIEW_
  CHtmlView::OnDocumentComplete(lpszURL);
#endif
}

void CMotdView::OnNavigateComplete2(LPCTSTR strURL) 
{
#ifdef HTML_VIEW_
  CHtmlView::OnNavigateComplete2(strURL);
#endif
}


void CMotdView::OnDraw(CDC*)
{
}
