#include "stdafx.h"

#pragma hdrstop

#include "Dib File Handler Targa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Include files

/*******************/
/* CDibFileHandler */
/*******************/

// Targa header
typedef struct _TgaHeader
{
    BYTE   IdLength;            // Image ID Field Length
    BYTE   CmapType;            // Color Map Type
    BYTE   ImageType;           // Image Type

    WORD   CmapIndex;           // First Entry Index
    WORD   CmapLength;          // Color Map Length
    BYTE   CmapEntrySize;       // Color Map Entry Size

    WORD   X_Origin;            // X-origin of Image
    WORD   Y_Origin;            // Y-origin of Image
    WORD   ImageWidth;          // Image Width
    WORD   ImageHeight;         // Image Height
    BYTE   PixelDepth;          // Pixel Depth
    BYTE   ImagDesc;            // Image Descriptor
} TGAHEADER;

// Targa image types
#define TGA_Null		0
#define TGA_Map			1
#define TGA_RGB			2
#define TGA_Mono		3
#define TGA_RLEMap		9
#define TGA_RLERGB		10
#define TGA_RLEMono		11
#define TGA_CompMap		32
#define TGA_CompMap4	33

/* Public part */

CDibFileHandlerTarga::CDibFileHandlerTarga ()
{
}

CDibFileHandlerTarga::~CDibFileHandlerTarga ()
{
}

BOOL CDibFileHandlerTarga::Open ( CDib *pdib, CString pathName )
{
	// Open targa file
	CFile targaFile;
		
	if ( !targaFile.Open ( pathName, CFile::modeRead ) )
	{
		AfxMessageBox ( "Targa: could not open file" );

		return FALSE;
	}

	// Read targa header
	TGAHEADER targaHeader;

	ZeroMemory ( &targaHeader, sizeof ( TGAHEADER ) );

	targaFile.Read ( &targaHeader.IdLength, 1 );
	targaFile.Read ( &targaHeader.CmapType, 1 );
	targaFile.Read ( &targaHeader.ImageType, 1 );
	targaFile.Read ( &targaHeader.CmapIndex, 2 );
	targaFile.Read ( &targaHeader.CmapLength, 2 );
	targaFile.Read ( &targaHeader.CmapEntrySize, 1 );
	targaFile.Read ( &targaHeader.X_Origin, 2 );
	targaFile.Read ( &targaHeader.Y_Origin, 2 );
	targaFile.Read ( &targaHeader.ImageWidth, 2 );
	targaFile.Read ( &targaHeader.ImageHeight, 2 );
	targaFile.Read ( &targaHeader.PixelDepth, 1 );
	targaFile.Read ( &targaHeader.ImagDesc, 1 );
		
	if ( targaHeader.ImageType != TGA_RGB )
	{
		AfxMessageBox ( "Targa: image type not yet supported" );

		return FALSE;
	}
			
	// Read targa bits (32 bit)
	if ( targaHeader.PixelDepth == 32 )
	{
		// Create dib
		int w = targaHeader.ImageWidth, h = targaHeader.ImageHeight;
				
		if ( !pdib->Create ( w, h ) )
		{
			AfxMessageBox ( "Targa: could not create dib" );

			return FALSE;
		}

		// Read bits
		targaFile.Read ( pdib->m_Bits, w * h * 4 );
	}
	// Read targa bits (24 bit)
	else if ( targaHeader.PixelDepth == 24 )
	{
		// Create dib
		int w = targaHeader.ImageWidth, h = targaHeader.ImageHeight;

		if ( !pdib->Create ( w, h ) )
		{
			AfxMessageBox ( "Targa: could not create dib" );

			return FALSE;
		}

		// Read bits
		BYTE *src = new BYTE[w * h * 3];
				
		if ( src == NULL )
		{
			pdib->Destroy ();

			AfxMessageBox ( "Targa: not enough memory" );

			return FALSE;
		}

		targaFile.Read ( src, w * h * 3 );

		// Copy bits
		BYTE *dst = (BYTE *)pdib->m_Bits;

		for ( int i = 0; i < w * h; i++ )
		{
			pdib->m_Bits[i] = RGB ( src[i * 3], src[i * 3 + 1], src[i * 3 + 2] );
		}

		delete [] src;
	}
	else
	{
		AfxMessageBox ( "Targa: bit depth not yet supported" );
		
		return FALSE;
	}

	return TRUE;
}

BOOL CDibFileHandlerTarga::Save ( CDib *pdib, CString pathName )
{
	return FALSE;
}