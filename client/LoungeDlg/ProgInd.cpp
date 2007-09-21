// ProgInd.cpp : implementation file
//

#include "stdafx.h"
#include "ProgInd.h"
#include "Filler.h"
#include "base/draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressIndicator

CProgressIndicator::CProgressIndicator( CFiller * pFiller ) : m_pFiller( pFiller )
{
	// Step 2 - Check and create m_pFiller
	if( m_pFiller == NULL )
	{
		m_pFiller  = new CLToRFiller;	
	}
	// Step 3 - Validate m_pFiller
	ASSERT( m_pFiller != NULL );
}

CProgressIndicator::~CProgressIndicator()
{
	// Step 1 - Check and delete m_pFiller
    delete m_pFiller;
}


BEGIN_MESSAGE_MAP(CProgressIndicator, CWnd)
	//{{AFX_MSG_MAP(CProgressIndicator)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressIndicator methods

CFiller * CProgressIndicator::GetFiller()
{
	// Step 1 - Return m_pFiller
	return m_pFiller;
}

INT CProgressIndicator::SetFiller( CFiller * pFiller )
{
	// Step 1 - Validate pFiller
	ASSERT( pFiller != NULL );
	// Step 2 - Check and delete m_pFiller
    delete m_pFiller;
	// Step 3 - Set m_pFiller to the parameter passed
	m_pFiller = pFiller;
	return SUCCESS;
}

INT CProgressIndicator::SetText( LPCSTR lpszText )
{
	// Step 1 - Delegate and forward this to CFiller object
	INT nRetVal = m_pFiller->SetFillerText( lpszText );
	Invalidate();
	return nRetVal;
}

INT CProgressIndicator::SetRange( INT nMinRange, INT nMaxRange )
{
	// Step 1 - Delegate and forward this to CFiller object
	return m_pFiller->SetFillerRange( nMinRange, nMaxRange );
}

INT CProgressIndicator::GetPos()
{
	// Step 1 - Delegate and forward this to CFiller object
	return m_pFiller->GetFillerPos();
}

INT CProgressIndicator::SetPos( INT nPos )
{
	// Step 1 - Delegate and forward this to CFiller object
	INT nOldPos = m_pFiller->SetFillerPos( nPos );

	CRect Rect;
	GetClientRect( & Rect );
    Rect.InflateRect(-1, -1);
    InvalidateRect(&Rect);
	return nOldPos;
}

/////////////////////////////////////////////////////////////////////////////
// CProgressIndicator message handlers

void CProgressIndicator::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Step 1 - Get the client Rectangle
	CRect Rect;
	GetClientRect( & Rect );

    Rect.InflateRect(-1, -1);
    // Step 2 - Call DoFill to do the filling, based on the current situation
	m_pFiller->DoFill( this, Rect );

    Rect.InflateRect(1, 1);
    // Draw a sunken edge around the control
    Base::DrawBumpRect(&dc, Rect,
                       GetSysColor(COLOR_3DHILIGHT),
                       GetSysColor(COLOR_3DDKSHADOW));
}
