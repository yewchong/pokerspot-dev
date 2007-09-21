#ifndef __DIB_FILE_HANDLER_TARGA_H__
#define __DIB_FILE_HANDLER_TARGA_H__

// Include files
#include "Dib File Handler.h"

/************************/
/* CDibFileHandlerTarga */
/************************/

class CDibFileHandlerTarga : 
	public CDibFileHandler  
	{
		public:
			CDibFileHandlerTarga ();
			virtual ~CDibFileHandlerTarga ();

			BOOL Open ( CDib *pdib, CString pathName );
			BOOL Save ( CDib *pdib, CString pathName );
	};

#endif // __DIB_FILE_HANDLER_TARGA_H__
