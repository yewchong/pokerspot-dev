// Dib.h: interface for the CDib class.
//
//	Creators : El Barto (ef00@luc.ac.be) and Chris (ef01@luc.ac.be)
//	Location : http://www.luc.ac.be/~ef00/ebgfx
//	Date : 19-05-98
//
//////////////////////////////////////////////////////////////////////

#ifndef __DIB_H__
#define __DIB_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Include files

/********/
/* CDib */
/********/

#define BGR(b, g ,r)  ((DWORD) (((BYTE) (b) | ((WORD) (g) << 8)) | (((DWORD) (BYTE) (r)) << 16))) 

class CDib  
{
	public:
		BITMAPINFO m_Info;
		LONG W, H;
		HBITMAP m_Bitmap;
		COLORREF *m_Bits;

		CDib ();
		~CDib ();

		// Create/Destroy
		BOOL Create ( int w, int h );
		BOOL Create ( CString pathName );
		void Destroy ();

		// Clipping
		inline BOOL CheckW ( int w );
		inline BOOL CheckW ( CDib *pdib );
		inline BOOL CheckH ( int h );
		inline BOOL CheckH ( CDib *pdib );
		inline BOOL CheckWH ( int w, int h );
		inline BOOL CheckWH ( CDib *pdib );

		inline void Resize ( int w, int h );
		inline void Resize ( CDib *pdib );

		inline BOOL IsValidX ( int x );
		inline BOOL IsValidY ( int y );

		inline void ClipX ( int &x );
		inline void ClipY ( int &y );
		inline BOOL ClipXYWH ( int &x, int &y, int &w, int &h );
		inline BOOL ClipX1YX2 ( int &x1, int &y, int &x2 );
		inline BOOL ClipXY1Y2 ( int &x, int &y1, int &y2 );

		// Fill handling
		void Fill ( BYTE r, BYTE g, BYTE b );
		void FillGlass ( BYTE r, BYTE g, BYTE b, BYTE a );
		void FillGlass50 ( BYTE r, BYTE g, BYTE b );
		void FillGlassStippled50 ( BYTE r, BYTE g, BYTE b );

		// Fill rect handling
		void FillRect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b );
		void FillGlassRect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, 
			BYTE a );
		void FillGlass50Rect ( int x, int y, int w, int h, BYTE r, BYTE g, BYTE b );
		void FillGlassStippled50Rect ( int x, int y, int w, int h, BYTE r, BYTE g, 
			BYTE b );

		// Bitmap handling
		void CopyToBitmap ( CDC *pdC, CBitmap *pbitmap );
		void CopyFromBitmap ( CDC *pdC, CBitmap *pbitmap );

		// DC handling
		void CopyToDCRect ( CDC *pdC, int x, int y );
		void CopyFromDCRect ( CDC *pdC, int x, int y, int w, int h );

		// Dib handling
		void CopyToDib ( CDib *pdib );
		void CopyFromDib ( CDib *pdib );

		void CopyToDibCK ( CDib *pdib, BYTE r, BYTE g, BYTE b );
		void CopyFromDibCK ( CDib *pdib, BYTE r, BYTE g, BYTE b );
		void CopyFromDibAlpha ( CDib *pdib );

		void AddDib ( CDib *pdib );
		void BlendDib ( CDib *pdib, BYTE a );
		void Blend50Dib ( CDib *pdib );
		void DarkenDib ( CDib *pdib );
		void DifferenceDib ( CDib *pdib );
		void LightenDib ( CDib *pdib );
		void MultiplyDib ( CDib *pdib );
		void ScreenDib ( CDib *pdib );

		// Dib rect handling
		void CopyToDibRect ( CDib *pdib, int x, int y );
		void CopyFromDibRect ( CDib *pdib, int x, int y );

		void CopyToDibRectCK ( CDib *pdib, int x, int y, BYTE r, BYTE g, BYTE b );
		void CopyFromDibRectCK ( CDib *pdib, int x, int y, BYTE r, BYTE g, BYTE b );
		void CopyFromDibRectAlpha ( CDib *pdib, int x, int y );

		void CopyStretchedFromDibRect ( CDib *pdib, int x, int y, int w, int h );
	
		void AddDibRect ( CDib *pdib, int x, int y );
		void BlendDibRect ( CDib *pdib, int x, int y, BYTE a );
		void Blend50DibRect ( CDib *pdib, int x, int y );
		void DarkenDibRect ( CDib *pdib, int x, int y );
		void DifferenceDibRect ( CDib *pdib, int x, int y );
		void LightenDibRect ( CDib *pdib, int x, int y );
		void MultiplyDibRect ( CDib *pdib, int x, int y );
		void ScreenDibRect ( CDib *pdib, int x, int y );

		// Primitive handling
		void HLine ( int x1, int y, int x2, BYTE r, BYTE g, BYTE b );
		void ClippedHLine ( int x1, int y, int x2, BYTE r, BYTE g, BYTE b );

		void VLine ( int x, int y1, int y2, BYTE r, BYTE g, BYTE b );
		void ClippedVLine ( int x, int y1, int y2, BYTE r, BYTE g, BYTE b );

		void Line ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b );
		void LineGlass ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, 
			BYTE a );
		void ClippedLine ( int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b );
};

// Clipping
inline BOOL CDib::CheckW ( int w ) 
{ 
	return ( W == w ); 
};

inline BOOL CDib::CheckW ( CDib *pdib ) 
{ 
	return ( W == pdib->W ); 
};

inline BOOL CDib::CheckH ( int h ) 
{ 
	return ( H == h ); 
};

inline BOOL CDib::CheckH ( CDib *pdib ) 
{ 
	return ( H == pdib->H ); 
};

inline BOOL CDib::CheckWH ( int w, int h ) 
{ 
	return ( ( W == w ) && ( H == h ) ); 
};

inline BOOL CDib::CheckWH ( CDib *pdib ) 
{ 
	return ( ( W == pdib->W ) && ( H == pdib->H ) ); 
};

inline void CDib::Resize ( int w, int h ) 
{ 
	if ( !CheckWH ( w, h ) )
		VERIFY ( Create ( w, h ) );
};

inline void CDib::Resize ( CDib *pdib ) 
{ 
	if ( !CheckWH ( pdib ) ) 
		VERIFY ( Create ( pdib->W, pdib->H ) ); 
};

inline BOOL CDib::IsValidX ( int x )
{
	return ( ( x >= 0 ) && ( x < W ) );
}

inline BOOL CDib::IsValidY ( int y )
{
	return ( ( y >= 0 ) && ( y < H ) );
}

inline void CDib::ClipX ( int &x )
{
	if ( x < 0 )
		x = 0;

	if ( x >= W )
		x = W - 1;
}

inline void CDib::ClipY ( int &y )
{
	if ( y < 0 )
		y = 0;

	if ( y >= H )
		y = H - 1;
}

inline BOOL CDib::ClipXYWH ( int &x, int &y, int &w, int &h )
{
	if ( ( x + w ) >= W ) 
		w = W - x;

	if ( x < 0 ) 
	{
		w += x;
		x = 0;
	}

	if ( ( y + h ) >= H )
		h = H - y;

	if ( y < 0 )
	{
		h += y;
		y = 0;
	}

	return ( ( w > 0 ) && ( h > 0 ) );
}

inline BOOL CDib::ClipX1YX2 ( int &x1, int &y, int &x2 )
{
	if ( x2 > W )
		x2 = W - 1;

	if ( x1 < 0 )
		x1 = 0;

	return ( ( x1 != x2 ) && IsValidY ( y ) );
}

inline BOOL CDib::ClipXY1Y2 ( int &x, int &y1, int &y2 )
{
	if ( y2 > H )
		y2 = H - 1;

	if ( y1 < 0 )
		y1 = 0;

	return ( ( y1 != y2 ) && IsValidX ( x ) );
}

#endif // __DIB_H__