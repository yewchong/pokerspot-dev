// Dib.cpp: implementation of the CDib class.
//
//	Creator : El Barto (ef00@luc.ac.be) and Chris (ef01@luc.ac.be)
//	Location : http://www.luc.ac.be/~ef00/ebgfx
//	Date : 19-05-98
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#include "Dib.h"

// Include files
#include "Assembler.h"
#include "Dib File Handler Targa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/********/
/* CDib */
/********/

/* Public part */

CDib::CDib () :
	W ( 0 ), H ( 0 ),
	m_Bitmap ( 0 ),
	m_Bits ( NULL )
{
	ZeroMemory ( &m_Info, sizeof ( BITMAPINFO ) );
}

CDib::~CDib ()
{
	Destroy ();
}

// Create/Destroy
BOOL CDib::Create ( int w, int h )
{
	Destroy ();

	m_Info.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
	m_Info.bmiHeader.biWidth = w;
	m_Info.bmiHeader.biHeight = -h;
	m_Info.bmiHeader.biPlanes = 1;
	m_Info.bmiHeader.biBitCount = 32; 
	m_Info.bmiHeader.biCompression = BI_RGB;
	m_Info.bmiHeader.biSizeImage = w * h * 4;
	
	m_Bitmap = CreateDIBSection ( NULL, &m_Info, DIB_RGB_COLORS, 
		(void * *)&m_Bits, NULL, NULL ); 

	if ( m_Bitmap == 0 )
	{
		Destroy ();

		return FALSE;
	}

	if ( m_Bits == NULL )
	{
		Destroy ();

		return FALSE;
	}

	W = w;
	H = h;

	return TRUE;
}

BOOL CDib::Create ( CString pathName )
{
	CString extension = pathName.Right ( 3 );

	if ( extension.CompareNoCase ( "tga" ) == 0 )
	{
		CDibFileHandlerTarga f;

		if ( !f.Open ( this, pathName ) )
			return FALSE;
	}

	return TRUE;
}

void CDib::Destroy ()
{
	m_Bits = NULL;

	if ( m_Bitmap )
	{
		DeleteObject ( m_Bitmap );

		m_Bitmap = NULL;
	}

	W = H = 0;

	ZeroMemory ( &m_Info, sizeof ( BITMAPINFO ) );
}

// Fill handling
void CDib::Fill ( BYTE r, BYTE g, BYTE b )
{
	// Fill dib
	DWSet ( m_Bits, BGR ( b, g, r ), W * H );
}

void CDib::FillGlass ( BYTE r, BYTE g, BYTE b, BYTE a )
{
	// Fill dib with glass
	BYTE *dst = (BYTE *)m_Bits;
	int len = W * H;
		
	while ( len-- )
	{	
		dst[0] = ( ( ( b - dst[0] ) * a ) + ( dst[0] << 8 ) ) >> 8;
		dst[1] = ( ( ( g - dst[1] ) * a ) + ( dst[1] << 8 ) ) >> 8;
		dst[2] = ( ( ( r - dst[2] ) * a ) + ( dst[2] << 8 ) ) >> 8;

		dst += 4;
	}
}

void CDib::FillGlass50 ( BYTE r, BYTE g, BYTE b )
{	
	// Fill dib with glass (50%)
	COLORREF *dst = m_Bits;
	int len = W * H;
	COLORREF color = BGR ( b, g, r );

	color >>= 1;
	color &= 0x007F7F7F;

	while ( len-- )
	{
		*dst = ( ( *dst >> 1 ) & 0x007F7F7F ) + color;

		dst++;
	}
}

void CDib::FillGlassStippled50 ( BYTE r, BYTE g, BYTE b )
{	
	// Fill dib with stippled glass (50%)
	COLORREF *dst = m_Bits;
	COLORREF color = BGR ( b, g, r );

	for ( int j = 0; j < H; j++ )
	{
		for ( int i = 0; i < W; i++ )
		{
			if ( ( i + j ) & 0x1 )
				dst[i] = color;
		}

		dst += W;
	}
}

// Fill rect handling
void CDib::FillRect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b )
{
	// Clip
	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	// Fill dib rect
	int t = W << 2;
	BYTE *dst = (BYTE *)m_Bits + y * t + ( x << 2 );
	COLORREF color = BGR ( b, g, r );

	while ( h-- )
	{
		DWSet ( dst, color, w );

		dst += t;
	}
}

void CDib::FillGlassRect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, 
	BYTE a )
{
	// Clip
	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	// Fill dib rect with glass
	int t = ( W - w ) << 2;
	BYTE *dst = (BYTE *)m_Bits + ( ( y * W + x ) << 2 );

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[0] = (BYTE)(( ( b - dst[0] ) * a + ( dst[0] << 8 ) ) >> 8);
			dst[1] = (BYTE)(( ( g - dst[1] ) * a + ( dst[1] << 8 ) ) >> 8);
			dst[2] = (BYTE)(( ( r - dst[2] ) * a + ( dst[2] << 8 ) ) >> 8);

			dst += 4;
		}

		dst += t;
	}
}

void CDib::FillGlass50Rect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b )
{
	// Clip
	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	// Fill dib rect with glass (50%)
	COLORREF *dst = m_Bits + y * W + x;
	COLORREF color = BGR ( b, g, r );

	color >>= 1;
	color &= 0x007F7F7F;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[i] = ( ( dst[i] >> 1 ) & 0x007F7F7F ) + color;	
		}

		dst += W;
	}
}

void CDib::FillGlassStippled50Rect ( int x, int y, int w, int h, BYTE r, BYTE g, 
	BYTE b )
{
	// Clip
	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	// Fill dib rect with stippled glass (50%)
	COLORREF *dst = m_Bits + y * W + x;
	COLORREF color = BGR ( b, g, r );

	for ( int j = 0; j < h; j++ )
	{
		for ( int i = 0; i < w; i++ )
		{
			if ( ( i + j ) & 0x1 )
				dst[i] = color;
		}

		dst += W;
	}
}

// Bitmap handling
void CDib::CopyToBitmap ( CDC *pdC, CBitmap *pbitmap )
{
	SetDIBits ( pdC->m_hDC, (HBITMAP)(*pbitmap), 0, H, m_Bits, &m_Info,
		DIB_RGB_COLORS );
}

void CDib::CopyFromBitmap ( CDC *pdC, CBitmap *pbitmap )
{
	GetDIBits ( pdC->m_hDC, (HBITMAP)(*pbitmap), 0, H, m_Bits, &m_Info, 
		DIB_RGB_COLORS );
}

// DC handling
void CDib::CopyToDCRect ( CDC *pdC, int x, int y )
{
	SetDIBitsToDevice ( pdC->m_hDC, x, y, W, H, 0, 0, 0, H, m_Bits, &m_Info, 
		DIB_RGB_COLORS );
}

void CDib::CopyFromDCRect ( CDC *pdC, int x, int y, int w, int h )
{
	ASSERT ( CheckWH ( w, h ) );
	
	CDC bufferDC;
	CBitmap bufferBitmap, *oldBitmap;

	bufferDC.CreateCompatibleDC ( pdC );
	bufferBitmap.CreateCompatibleBitmap ( pdC, w, h );

	oldBitmap = bufferDC.SelectObject ( &bufferBitmap );

	bufferDC.FillSolidRect ( 0, 0, w, h, 0 );
	bufferDC.BitBlt ( 0, 0, w, h, pdC, x, y, SRCCOPY );
	bufferDC.SelectObject ( oldBitmap );

	GetDIBits ( pdC->m_hDC, (HBITMAP)bufferBitmap, 0, h, m_Bits, &m_Info, 
		DIB_RGB_COLORS );
}

// Dib handling
void CDib::CopyToDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	// Copy to dib
	DWCopy ( pdib->m_Bits, m_Bits, W * H );
}

void CDib::CopyFromDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	// Copy from dib
	DWCopy ( m_Bits, pdib->m_Bits, W * H );
}

void CDib::CopyFromDibCK ( CDib *pdib, BYTE r, BYTE g, BYTE b )
{
	ASSERT ( CheckWH ( pdib ) );
	
	// Copy from dib with colorkey
	DWCopyCK ( m_Bits, pdib->m_Bits, W * H, BGR ( b, g, r ) );
}

void CDib::CopyFromDibAlpha ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	ASSERT ( FALSE );
}

void CDib::AddDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
	WORD sum;
		
	while ( len-- )
	{
		sum = src[0] + dst[0];
		dst[0] = (BYTE)(( sum > 255 ) ? 255 : sum);

		sum = src[1] + dst[1];
		dst[1] = (BYTE)(( sum > 255 ) ? 255 : sum);

		sum = src[2] + dst[2];
		dst[2] = (BYTE)(( sum > 255 ) ? 255 : sum);

		dst += 4;
		src += 4;
	}
}

void CDib::BlendDib ( CDib *pdib, BYTE a )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		dst[0] = (BYTE)(( ( src[0] - dst[0] ) * a + ( dst[0] << 8 ) ) >> 8);

		dst[1] = (BYTE)(( ( src[1] - dst[1] ) * a + ( dst[1] << 8 ) ) >> 8);

		dst[2] = (BYTE)(( ( src[2] - dst[2] ) * a + ( dst[2] << 8 ) ) >> 8);

		dst += 4;
		src += 4;
	}
}

void CDib::Blend50Dib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	COLORREF *dst = m_Bits;
	COLORREF *src = pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		*dst = ( ( *dst >> 1 ) & 0x007F7F7F ) + ( ( *src >> 1 ) & 0x007F7F7F );	

		dst++;
		src++;
	}
}

void CDib::DarkenDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		if ( src[0] < dst[0] )
			dst[0] = src[0];

		if ( src[1] < dst[1] )
			dst[1] = src[1];

		if ( src[2] < dst[2] )
			dst[2] = src[2];

		dst += 4;
		src += 4;
	}
}

void CDib::DifferenceDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
	SHORT dif;
		
	while ( len-- )
	{
		dif = src[0] - dst[0];
		dst[0] = (BYTE)(( dif < 0 ) ? -dif : dif);

		dif = src[1] - dst[1];
		dst[1] = (BYTE)(( dif < 0 ) ? -dif : dif);

		dif = src[2] - dst[2];
		dst[2] = (BYTE)(( dif < 0 ) ? -dif : dif);

		dst += 4;
		src += 4;
	}
}

void CDib::LightenDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		if ( src[0] > dst[0] )
			dst[0] = src[0];

		if ( src[1] > dst[1] )
			dst[1] = src[1];

		if ( src[2] > dst[2] )
			dst[2] = src[2];

		dst += 4;
		src += 4;
	}
}

void CDib::MultiplyDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		dst[0] = (BYTE)(( src[0] * dst[0] ) >> 8);
		dst[1] = (BYTE)(( src[1] * dst[1] ) >> 8);
		dst[2] = (BYTE)(( src[2] * dst[2] ) >> 8);

		dst += 4;
		src += 4;
	}
}

void CDib::ScreenDib ( CDib *pdib )
{
	ASSERT ( CheckWH ( pdib ) );

	BYTE *dst = (BYTE *)m_Bits;
	BYTE *src = (BYTE *)pdib->m_Bits;
	int len = W * H;
		
	while ( len-- )
	{
		dst[0] = (BYTE)(255 - ( ( ( 255 - src[0] ) * ( 255 - dst[0] ) ) >> 8 ));
		dst[1] = (BYTE)(255 - ( ( ( 255 - src[1] ) * ( 255 - dst[1] ) ) >> 8 ));
		dst[2] = (BYTE)(255 - ( ( ( 255 - src[2] ) * ( 255 - dst[2] ) ) >> 8 ));

		dst += 4;
		src += 4;
	}
}

// Dib rect handling
void CDib::CopyToDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	ASSERT ( pdib->CheckWH ( w, h ) );

	// Copy to dib rect
	COLORREF *dst = pdib->m_Bits;
	COLORREF *src = m_Bits + y * W + x;

	while ( h-- )
	{
		DWCopy ( dst, src, W );
		
		dst += pdib->W;
		src += W;
	}
}

void CDib::CopyFromDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Copy from dib rect
	COLORREF *dst = m_Bits + cy * W + cx;
	COLORREF *src = pdib->m_Bits + ( cy - y ) * pdib->W + ( cx - x );

	while ( h-- )
	{
		DWCopy ( dst, src, w );

		dst += W;
		src += pdib->W;
	}
}

void CDib::CopyToDibRectCK ( CDib *pdib, int x, int y, BYTE r, BYTE g, BYTE b )
{
	// Clip
	int w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( x, y, w, h ) )
		return;

	ASSERT ( pdib->CheckWH ( w, h ) );

	// Copy to dib rect colorkey
	COLORREF *dst = pdib->m_Bits;
	COLORREF *src = m_Bits + y * W + x;
	COLORREF cK = BGR ( b, g, r );

	while ( h-- )
	{
		DWCopyCK ( dst, src, W, cK );
		
		dst += pdib->W;
		src += W;
	}
}

void CDib::CopyFromDibRectCK ( CDib *pdib, int x, int y, BYTE r, BYTE g, BYTE b )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Copy from dib rect with colorkey
	COLORREF *dst = m_Bits + cy * W + cx;
	COLORREF *src = pdib->m_Bits + ( cy - y ) * pdib->W + ( cx - x );
	COLORREF cK = BGR ( b, g, r );

	while ( h-- )
	{
		DWCopyCK ( dst, src, w, cK );

		dst += W;
		src += pdib->W;
	}
}

void CDib::CopyFromDibRectAlpha ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Copy from dib rect with alpha
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[0] = (BYTE)(( ( src[0] - dst[0] ) * src[3] + ( dst[0] << 8 ) ) >> 8);
			dst[1] = (BYTE)(( ( src[1] - dst[1] ) * src[3] + ( dst[1] << 8 ) ) >> 8);
			dst[2] = (BYTE)(( ( src[2] - dst[2] ) * src[3] + ( dst[2] << 8 ) ) >> 8);

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::CopyStretchedFromDibRect ( CDib *pdib, int x, int y, int w, int h )
{
	// Clip
	int cx = x, cy = y;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Copy (stretched) from dib rect
	COLORREF *dst = m_Bits + cy * W + cx;
	COLORREF *src = pdib->m_Bits + ( cy - y ) * pdib->W + ( cx - x );

	int sw = (int)(( (float)pdib->W / (float)w ) * 65536.0),
		sh = (int)(( (float)pdib->H / (float)h ) * 65536.0),
		t1, t2 = W - w;

	for ( int j = 0; j < h; j++ )
	{
		t1 = ( j * sh >> 16 ) * pdib->W;

		for ( int i = 0; i < w; i++ )
		{
			*dst = src[t1 + ( i * sw >> 16 )];

			dst++;
		}

		dst += t2;
	}
}
	
void CDib::AddDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Add dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	WORD sum;
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			sum = src[0] + dst[0];
			dst[0] = (BYTE)(( sum > 255 ) ? 255 : sum);

			sum = src[1] + dst[1];
			dst[1] = (BYTE)(( sum > 255 ) ? 255 : sum);

			sum = src[2] + dst[2];
			dst[2] = (BYTE)(( sum > 255 ) ? 255 : sum);

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::BlendDibRect ( CDib *pdib, int x, int y, BYTE a )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Blend dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[0] = (BYTE)(( ( src[0] - dst[0] ) * a + ( dst[0] << 8 ) ) >> 8);

			dst[1] = (BYTE)(( ( src[1] - dst[1] ) * a + ( dst[1] << 8 ) ) >> 8);

			dst[2] = (BYTE)(( ( src[2] - dst[2] ) * a + ( dst[2] << 8 ) ) >> 8);

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::Blend50DibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Blend dib rect (50%)
	COLORREF *dst = m_Bits + cy * W + cx;
	COLORREF *src = pdib->m_Bits + ( cy - y ) * pdib->W + ( cx - x );
	int t1 = W - w, t2 = pdib->W - w;
	
	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			*dst = ( ( *dst >> 1 ) & 0x007F7F7F ) + ( ( *src >> 1 ) & 0x007F7F7F );

			dst++;
			src++;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::DarkenDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Darken dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			if ( src[0] < dst[0] )
				dst[0] = src[0];

			if ( src[1] < dst[1] )
				dst[1] = src[1];

			if ( src[2] < dst[2] )
				dst[2] = src[2];

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::DifferenceDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Difference dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;
	SHORT dif;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dif = src[0] - dst[0];
			dst[0] = (BYTE)(( dif < 0 ) ? -dif : dif);

			dif = src[1] - dst[1];
			dst[1] = (BYTE)(( dif < 0 ) ? -dif : dif);

			dif = src[2] - dst[2];
			dst[2] = (BYTE)(( dif < 0 ) ? -dif : dif);

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::LightenDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Lighten dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			if ( src[0] > dst[0] )
				dst[0] = src[0];

			if ( src[1] > dst[1] )
				dst[1] = src[1];

			if ( src[2] > dst[2] )
				dst[2] = src[2];

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::MultiplyDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Multiply dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[0] = (BYTE)(( src[0] * dst[0] ) >> 8);
			dst[1] = (BYTE)(( src[1] * dst[1] ) >> 8);
			dst[2] = (BYTE)(( src[2] * dst[2] ) >> 8);

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

void CDib::ScreenDibRect ( CDib *pdib, int x, int y )
{
	// Clip
	int cx = x, cy = y, w = pdib->W, h = pdib->H;

	if ( !ClipXYWH ( cx, cy, w, h ) )
		return;

	// Screen dib rect
	BYTE *dst = (BYTE *)m_Bits + ( ( cy * W + cx ) << 2 );
	BYTE *src = (BYTE *)pdib->m_Bits + ( ( ( cy - y ) * pdib->W + ( cx - x ) ) << 2 );
	int t1 = ( W - w ) << 2, t2 = ( pdib->W - w ) << 2;

	while ( h-- )
	{
		for ( int i = 0; i < w; i++ )
		{
			dst[0] = (BYTE)(255 - ( ( ( 255 - src[0] ) * ( 255 - dst[0] ) ) >> 8 ));
			dst[1] = (BYTE)(255 - ( ( ( 255 - src[1] ) * ( 255 - dst[1] ) ) >> 8 ));
			dst[2] = (BYTE)(255 - ( ( ( 255 - src[2] ) * ( 255 - dst[2] ) ) >> 8 ));

			dst += 4;
			src += 4;
		}

		dst += t1;
		src += t2;
	}
}

// Primitive handling
void CDib::HLine ( int x1, int y, int x2, BYTE r, BYTE g, BYTE b )
{
	// Horizontal line
	DWSet ( &m_Bits[y * W + x1], BGR ( b, g, r ), x2 - x1 + 1 );
}

void CDib::ClippedHLine ( int x1, int y, int x2, BYTE r, BYTE g, BYTE b )
{
	// Clip
	if ( !ClipX1YX2 ( x1, y, x2 ) )
		return;

	// Horizontal line clipped
	DWSet ( &m_Bits[y * W + x1], BGR ( b, g, r ), x2 - x1 + 1 );
}

void CDib::VLine ( int x, int y1, int y2, BYTE r, BYTE g, BYTE b )
{
	// Vertical line
	PVOID dst = &m_Bits[y1 * W + x];
	DWORD data = BGR ( b, g, r );
	DWORD len = y2 - y1 + 1;

	DWORD w = ( W << 2 ) - 4;

	__asm
	{
		MOV EDI, dst
		MOV EAX, data
		MOV ECX, len
		MOV EDX, w
l1:
		CMP ECX, 0
		JE l2

		STOSD

		ADD EDI, EDX
		DEC ECX

		JMP l1
l2:
	}
}

void CDib::ClippedVLine ( int x, int y1, int y2, BYTE r, BYTE g, BYTE b )
{
	// Clip
	if ( !ClipXY1Y2 ( x, y1, y2 ) )
		return;

	// Vertical line clipped
	PVOID dst = &m_Bits[y1 * W + x];
	DWORD data = BGR ( b, g, r );
	DWORD len = y2 - y1 + 1;

	DWORD w = ( W << 2 ) - 4;

	__asm
	{
		MOV EDI, dst
		MOV EAX, data
		MOV ECX, len
		MOV EDX, w
l1:
		CMP ECX, 0
		JE l2

		STOSD

		ADD EDI, EDX
		DEC ECX

		JMP l1
l2:
	}
}

#define CLIPCODECS(x,y) (((x<0)?8:0)|((x>=W)?4:0)|((y<0)?2:0)|((y>=H)?1:0))

void CDib::Line ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b )
{
	int dx, ax, sx, dy, ay, sy, x, y, d;
	COLORREF color = BGR ( b, g, r );
	
	dx = x2 - x1;
	ax = abs ( dx ) << 1;
	sx = ( dx < 0 ) ? -1 : 1;
	dy = y2 - y1;
	ay = abs ( dy ) << 1;
	sy = ( dy < 0 ) ? -1 : 1;
	x = x1;
	y = y1;
	
	if ( ax > ay )
	{
		d = ay - ( ax >> 1 );

		while ( x != x2 )
		{
			m_Bits[y * W + x] = color;

			if ( d >= 0 )
			{
				y += sy;
				d -= ax;
			}

			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - ( ay >> 1 );

		while ( y != y2 )
		{
			m_Bits[y * W + x] = color;

			if ( d >= 0 )
			{
				x += sx;
				d -= ay;
			}

			y += sy;
			d += ax;
		}
	}
}

void CDib::LineGlass ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b,
	BYTE a )
{
	int dx, ax, sx, dy, ay, sy, x, y, d;
	COLORREF color = BGR ( b, g, r );
	BYTE *dst = (BYTE *)m_Bits;
	
	dx = x2 - x1;
	ax = abs ( dx ) << 1;
	sx = ( dx < 0 ) ? -1 : 1;
	dy = y2 - y1;
	ay = abs ( dy ) << 1;
	sy = ( dy < 0) ? -1 : 1;
	x = x1;
	y = y1;
	
	if ( ax > ay )
	{
		d = ay - ( ax >> 1);

		while ( x != x2 )
		{
			dst[( y * W + x ) << 2] = (BYTE)(( ( b - dst[( y * W + x ) << 2] ) * a + 
				( dst[( y * W + x ) << 2] << 8 ) ) >> 8);
			dst[( ( y * W + x ) << 2 ) + 1] = (BYTE)(( ( g - dst[( ( y * W + x ) << 2 ) + 1] ) * a + 
				( dst[( ( y * W + x ) << 2 ) + 1] << 8 ) ) >> 8);
			dst[( ( y * W + x ) << 2 ) + 2] = (BYTE)(( ( r - dst[( ( y * W + x ) << 2 ) + 2] ) * a +
				( dst[( ( y * W + x ) << 2 ) + 2] << 8 ) ) >> 8);

			if ( d >= 0 )
			{
				y += sy;
				d -= ax;
			}

			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - ( ay >> 1);

		while ( y != y2 )
		{
			dst[( y * W + x ) << 2] = (BYTE)(( ( b - dst[( y * W + x ) << 2] ) * a + 
				( dst[( y * W + x ) << 2] << 8 ) ) >> 8);
			dst[( ( y * W + x ) << 2 ) + 1] = (BYTE)(( ( g - dst[( ( y * W + x ) << 2 ) + 1] ) * a + 
				( dst[( ( y * W + x ) << 2 ) + 1] << 8 ) ) >> 8);
			dst[( ( y * W + x ) << 2 ) + 2] = (BYTE)(( ( r - dst[( ( y * W + x ) << 2 ) + 2] ) * a +
				( dst[( ( y * W + x ) << 2 ) + 2] << 8 ) ) >> 8);

			if ( d >= 0 )
			{
				x += sx;
				d -= ay;
			}

			y += sy;
			d += ax;
		}
	}
}

void CDib::ClippedLine ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b )
{
	int c1 = CLIPCODECS ( x1, y1 ), c2 = CLIPCODECS ( x2, y2 );
    
	int dx, dy;

    while ( ( c1 | c2 ) != 0 )
    {
		if ( ( c1 & c2 ) != 0 )
			return;

		dx = x2 - x1;
		dy = y2 - y1;

		if ( c1 != 0 )
		{  
			if ( ( c1 & 8 ) == 8)
			{
				y1 += ( 0 - x1 ) * dy / dx;
				x1 = 0;
			}  
			else if ( ( c1 & 4 ) == 4)
			{
				y1 += ( W - 1 - x1 ) * dy / dx;
				x1 = W - 1;
			}  
            else if ( ( c1 & 2 ) == 2 )
			{
				x1 += ( 0 - y1 ) * dx / dy;
				y1 = 0;
			}  
            else if ( ( c1 & 1 ) == 1)
			{
				x1 += ( H - y1 - 1 ) * dx / dy;
				y1 = H - 1;
			}  

            c1 = CLIPCODECS ( x1, y1 );
         }  
         else if ( c2 != 0 )
         {
			if ( ( c2 & 8 ) == 8 )
			{
				y2 += ( 0 - x2 ) * dy / dx;
				x2 = 0;
			}  
			else if ( ( c2 & 4 ) == 4 )
			{
				y2 += ( W - 1 - x2 ) * dy / dx;
				x2 = W - 1;
			}  
            else if ( ( c2 & 2 ) == 2 )
			{
				x2 += ( 0 - y2 ) * dx / dy;
				y2 = 0;
			}  
            else if ( ( c2 & 1 ) == 1 )
			{
				x2 += ( H - 1 - y2 ) * dx / dy;
				y2 = H - 1;
			}  

            c2 = CLIPCODECS ( x2, y2 );
		}  
	}
	
	int ax, sx, ay, sy, x, y, d;
	COLORREF color = BGR ( b, g, r );
	
	dx = x2 - x1;
	ax = abs ( dx ) << 1;
	sx = ( dx < 0 ) ? -1 : 1;
	dy = y2 - y1;
	ay = abs ( dy ) << 1;
	sy = ( dy < 0 ) ? -1 : 1;
	x = x1;
	y = y1;
	
	if ( ax > ay )
	{
		d = ay - ( ax >> 1 );

		while ( x != x2 )
		{
			m_Bits[y * W + x] = color;

			if ( d >= 0 )
			{
				y += sy;
				d -= ax;
			}

			x += sx;
			d += ay;
		}
	}
	else
	{
		d = ax - ( ay >> 1 );

		while ( y != y2 )
		{
			m_Bits[y * W + x] = color;

			if ( d >= 0 )
			{
				x += sx;
				d -= ay;
			}

			y += sy;
			d += ax;
		}
	}
}