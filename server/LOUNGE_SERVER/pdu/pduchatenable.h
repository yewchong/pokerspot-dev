#ifndef __lounge_pduchatenable_h__
#define __lounge_pduchatenable_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduChatEnable
  Created  : 05/19/2000

  OVERVIEW : Send-only chat enable PDU.

 ****************************************************************************/

#include "pdu.h"

enum EnableChatFlag
{
  CE_ChatEnabled       = 0,
  CE_TournamentSeating = 1,
  CE_Bandwidth         = 2,
  CE_YourChatDisabled  = 4,
  CE_ChatDisabled      = 8,
  CE_Message           = 16
};

class CpduChatEnable : public Cpdu
{
public:
  CpduChatEnable();
  int sendChatEnable(SOCKET sd,
                     u_int16_t flag,
                     const char* message);
  int broadcastChatEnable(u_int16_t flag,
                          const char* message);
};

#endif
