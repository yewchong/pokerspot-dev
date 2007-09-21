#ifndef __DIB_FILE_HANDLER_H__
#define __DIB_FILE_HANDLER_H__

// Include files
#include "Dib.h"

/*******************/
/* CDibFileHandler */
/*******************/

class CDibFileHandler  
{
	public:
		CDibFileHandler ();
		virtual ~CDibFileHandler ();

		virtual BOOL Open ( CDib *pdib, CString pathName );
		virtual BOOL Save ( CDib *pdib, CString pathName );
};

#endif // __FILE_HANDLER_H__