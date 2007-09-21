/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class PDUChatEnable
  Created  : 05/18/2001

  OVERVIEW : Chat enable/disable pdu.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduchatenable.h"
#include "ui/global.h"
#include "ui/chatview.h"


BEGIN_NAMESPACE(Network)

void PDUEnableChat::execute(GameState*)
{
  if (flags_ == 0)
  {
    // Enable chat
    Global::Instance().enableChat("");
    //CChatView::Instance()->addDealerMessage("system: Chat is enabled",
    //                                      CChatView::CF_GameNormal);
  }
  if (flags_ == EC_TournamentSeating)
  {
    Global::Instance().enableChat("The chat function is off while tournament seating is in progress.");
  }
  else if (flags_ == EC_Bandwidth)
  {
    Global::Instance().enableChat("The chat function is currently off due to bandwidth limitations.");
  }
  else if (flags_ == EC_YourChatDisabled)
  {
    Global::Instance().enableChat("Your chat has been temporarily disabled.");
  }
  else if (flags_ == EC_ChatDisabled)
  {
    Global::Instance().enableChat("The chat function is temporarily disabled.");
  }
  else if (flags_ == EC_Message)
  {
    Global::Instance().enableChat(message_);
  }
}


END_NAMESPACE(Network)


