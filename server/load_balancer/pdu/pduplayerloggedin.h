#ifndef __lb_pduplayerloggedin_h__
#define __lb_pduplayerloggedin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedIn
  Created  : 10/04/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             a successfull player login.

 ****************************************************************************/


#include "pdu/pdu.h"

struct pdu_playerloggedin
{
  char name[PDU_STRINGSIZE];
};


class CpduPlayerLoggedIn : public CPDU
{
public:
  CpduPlayerLoggedIn();
  
  const char* getUsername() const;  
  void grok(SOCKET, CLounge*);
};


#endif
