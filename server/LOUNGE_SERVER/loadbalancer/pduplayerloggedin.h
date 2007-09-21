#ifndef __lounge_pduplayerloggedin_h__
#define __lounge_pduplayerloggedin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedIn
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             successfull player login

 ****************************************************************************/

#include "pdu/pdu.h"

struct pdu_playerloggedin
{
  char username[PDU_STRINGSIZE];
};

class CpduPlayerLoggedIn : public Cpdu
{
public:
  CpduPlayerLoggedIn();
  int sendPlayerLoggedIn(SOCKET sd,
                         const char* username);
};



#endif