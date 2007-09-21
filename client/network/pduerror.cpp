/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pduerror.cpp
  Created  : 12.12.1999

  OVERVIEW : Error PDUs.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduerror.h"
#include "ui/tableview.h"

namespace
{
  LPCTSTR g_szErrFmt = _T("The server returned an error:\n\n%s");
}


BEGIN_NAMESPACE(Network)


void PDUErrorClient::execute(Base::GameState*)
{
  CString msg;
  if (message_ != NULL && *message_ != '\0')
    msg.Format(g_szErrFmt, message_);
  else
    msg.Format(g_szErrFmt, "no message");

  if (AfxGetMainWnd())
    AfxGetMainWnd()->MessageBox(msg, NULL, MB_OK|MB_ICONEXCLAMATION);
}

END_NAMESPACE(Network)