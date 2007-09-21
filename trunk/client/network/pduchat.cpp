/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUChat
  Created  : 10.11.1999

  OVERVIEW : Chat PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduchat.h"
#include "ui/chatview.h"


BEGIN_NAMESPACE(Network)


void PDUChat::execute(Base::GameState*)
{
  if (CChatView::Instance())
  {
    CString msg, n = MakeString(username_);
    msg.Format("%s: %s", (LPCSTR)n, message_);

    if (_strnicmp(username_, "system", 6) == 0 ||
        _strnicmp(username_, "tournament", 10) == 0)
    {
      CChatView::Instance()->addDealerMessage(msg, CChatView::CF_GameNormal);
    }
    else
    {
      CChatView::Instance()->addUserMessage(msg, 0);
    }    
  }
}


END_NAMESPACE(Network)