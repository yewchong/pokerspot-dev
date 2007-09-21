#include "stdafx.h"

#pragma hdrstop

#include "Dib File Handler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Include files

/*******************/
/* CDibFileHandler */
/*******************/

/* Public part */

CDibFileHandler::CDibFileHandler ()
{
}

CDibFileHandler::~CDibFileHandler ()
{
}

BOOL CDibFileHandler::Open ( CDib *pdib, CString pathName )
{
	return FALSE;
}

BOOL CDibFileHandler::Save ( CDib *pdib, CString pathName )
{
	return FALSE;
}