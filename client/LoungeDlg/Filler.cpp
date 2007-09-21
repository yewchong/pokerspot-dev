#include "Stdafx.h"
#include "Filler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void DrawGradient(CDC* pDC,
                  const CRect& rect,
                  COLORREF clr1,
                  COLORREF clr2,
                  bool ltor);

// CFiller - Implementation

CFiller::CFiller()
{
	m_csText			= _T("");
	m_nMinVal			= 0;
	m_nMaxVal			= 10;
	m_nPos				= 0;
	m_FillerColor		= RGB( 0, 0, 255 );
//    m_FillerColor       = RGB(190, 160, 70);
	m_FillerBkColor		= GetSysColor(COLOR_BTNFACE);//RGB( 164, 164, 164 );
	m_FillerTextColor	= RGB( 255, 255, 255 );
}

CFiller::~CFiller()
{
}

INT CFiller::SetFillerText( LPCSTR lpszText )
{
	// Step 1 - Set Filler Text
	m_csText = lpszText;
	return SUCCESS;
}

INT CFiller::SetFillerRange( INT nMinVal, INT nMaxVal )
{
	// Step 1 - Set MinVal and MaxVal after validating
	if( nMaxVal <= nMinVal )
	{
		return FAILURE;
	}
	m_nMinVal = nMinVal;
	m_nMaxVal = nMaxVal;
	return SUCCESS;
}

INT CFiller::GetFillerPos()
{
	// Step 1 - Return m_nPos
	return m_nPos;
}

INT CFiller::SetFillerPos( INT nPos )
{
	// Step 1 - Validate pos before setting
	if( nPos < m_nMinVal || nPos > m_nMaxVal )
	{
		return FAILURE;
	}
	// Step 2 - Store old filler pos
	INT nOldPos = m_nPos;
	// Step 3 - Set m_nPos
	m_nPos = nPos;
	// Step 4 - Return old filler pos
	return nOldPos;
}

COLORREF CFiller::SetFillerColor( COLORREF & rFillerColor )
{
	// Step 1 - Store old color
	COLORREF OldFillerColor = m_FillerColor;
	// Step 2 - Set m_FillerColor
	m_FillerColor = rFillerColor;
	// Step 3 - Return old filler color
	return OldFillerColor;
}

COLORREF CFiller::SetFillerBkColor( COLORREF & rFillerBkColor )
{
	// Step 1 - Store old bk color
	COLORREF OldFillerBkColor = m_FillerBkColor;
	// Step 2 - Set m_FillerBkColor
	m_FillerBkColor = rFillerBkColor;
	// Step 3 - Return old filler bk color
	return OldFillerBkColor;
}

COLORREF CFiller::SetFillerTextColor( COLORREF & rFillerTextColor )
{
	// Step 1 - Store old text color
	COLORREF OldFillerTextColor = m_FillerTextColor;
	// Step 2 - Set m_FillerTextColor
	m_FillerTextColor = rFillerTextColor;
	// Step 3 - Return old filler text color
	return OldFillerTextColor;
}

// CLtoRFiller - Implementation

CLToRFiller::CLToRFiller() : CFiller()
{
}

CLToRFiller::~CLToRFiller()
{
}

INT CLToRFiller::DoFill( CWnd * pWnd, const CRect & rRect )
{
	CRect Rect = rRect;
	// Step 1 - Validate window pointer
	ASSERT( IsWindow( pWnd->m_hWnd ) );

	// Step 2 - Calculate Fill Area
	INT nRight = 0;
	nRight = ( rRect.Width() * m_nPos ) / ( m_nMaxVal - m_nMinVal );
	CRect FillArea( rRect.left, rRect.top, nRight, rRect.bottom );
	CRect RemainingArea( nRight, rRect.top, rRect.right, rRect.bottom );
	CDC * pDC = pWnd->GetDC();
    DrawGradient(pDC, FillArea, m_FillerColor, m_FillerBkColor, true);
	pDC->FillSolidRect( RemainingArea, m_FillerBkColor );

	pWnd->ReleaseDC( pDC );
	return SUCCESS;
}


// CRToLFiller - Implementation

CRToLFiller::CRToLFiller() : CFiller()
{
}

CRToLFiller::~CRToLFiller()
{
}

INT CRToLFiller::DoFill( CWnd * pWnd, const CRect & rRect )
{
	CRect Rect = rRect;
	// Step 1 - Validate window pointer
	ASSERT( IsWindow( pWnd->m_hWnd ) );

	// Step 2 - Calculate Fill Area
	INT nLeft = 0;
	nLeft = ( rRect.Width() * m_nPos ) / ( m_nMaxVal - m_nMinVal );
	nLeft = rRect.Width() - nLeft;
    if (nLeft < rRect.left)
        nLeft = rRect.left;
	CRect FillArea( nLeft, rRect.top, rRect.right, rRect.bottom );
	CRect RemainingArea( rRect.left, rRect.top, nLeft, rRect.bottom );
	CDC * pDC = pWnd->GetDC();
    DrawGradient(pDC, FillArea, m_FillerColor, m_FillerBkColor, false);
	pDC->FillSolidRect( RemainingArea, m_FillerBkColor );
/*	
	// Step 3 - Display text provided or the default %
	if( m_csText.IsEmpty() == FALSE )
	{
		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_FillerTextColor );
		pDC->DrawText( m_csText, m_csText.GetLength(), 
			Rect, DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE ); 
 	}
	else if( m_nPos > 0 )
	{
		CHAR szText[ 512 ];

		memset( szText, 0, sizeof( szText ) );
		INT nPercent = ( INT )( ( ( double ) m_nPos / ( double ) m_nMaxVal ) * 100 );
		sprintf( szText, "%d%%", nPercent );
		pDC->SetBkMode( TRANSPARENT );
		pDC->SetTextColor( m_FillerTextColor );
		pDC->DrawText( szText, strlen( szText ), 
			Rect, DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE ); 
	}
*/
	pWnd->ReleaseDC( pDC );
	return SUCCESS;
}


void DrawGradient(CDC* pDC,
                  const CRect& rectClient,
                  COLORREF clr1,
                  COLORREF clr2,
                  bool ltor)
{ 
  HDC hDC = pDC->GetSafeHdc(); 
  CRect rectFill;          // Rectangle for filling band
  float fStep;            // How large is each band?
  HBRUSH hBrush;
  int iOnBand;  // Loop index

  // Determine how large each band should be in order to cover the
  // client with 256 bands (one for every color intensity level)
  fStep = (float)(rectClient.right - rectClient.left) / 256.0f;

  // Linear interpolation of the color
  BYTE r1 = GetRValue(clr1);
  BYTE g1 = GetGValue(clr1);
  BYTE b1 = GetBValue(clr1);
  BYTE r2 = GetRValue(clr2);
  BYTE g2 = GetGValue(clr2);
  BYTE b2 = GetBValue(clr2);
  float rstep = float(r1 - r2) / 255.0f;
  float gstep = float(g1 - g2) / 255.0f;
  float bstep = float(b1 - b2) / 255.0f;
  float r = r1, g = g1, b = b1;

  // Start filling bands
  for (iOnBand = 0; iOnBand < 256; iOnBand++) {

    // Set the location of the current band
    if (ltor)
    {
        SetRect(&rectFill,
                rectClient.right - (int)((iOnBand+1) * fStep), // Upper left X
                rectClient.top,                                             // Upper left Y
                rectClient.right - (int)(iOnBand * fStep),     // Lower right X
                rectClient.bottom);                          // Lower right Y

    }
    else
    {
        SetRect(&rectFill,
                rectClient.left + (int)((iOnBand+1) * fStep), // Upper left X
                rectClient.top,                                            // Upper left Y
                rectClient.left + (int)(iOnBand * fStep),     // Lower right X
                rectClient.bottom);                         // Lower right Y
    }

    hBrush = CreateSolidBrush(RGB((BYTE)r, (BYTE)g, (BYTE)b));

    // Linear interpolation of the color
    r -= rstep;
    g -= gstep;
    b -= bstep;
    if (r > 255.0f)
        r = 255.0f;
    if (g > 255.0f)
        g = 255.0f;
    if (b > 255.0f)
        b = 255.0f;
    if (r < 0.0f)
        r = 0.0f;
    if (g < 0.0f)
        g = 0.0f;
    if (b < 0.0f)
        b = 0.0f;

    // Create a brush with the appropriate color for this band
//    hBrush = CreateSolidBrush(RGB(0, 0, (255 - iOnBand)));

    // Fill the rectangle
    FillRect(hDC, &rectFill, hBrush);

    // Get rid of the brush you created
    DeleteObject(hBrush);
  };
} 