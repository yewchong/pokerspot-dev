#ifndef __lb_pduredirectlogin_h__
#define __lb_pduredirectlogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRedirectLogin
  Created  : 09/30/2001

  OVERVIEW : This class is used by the Load Balancer to 
             redirect a player to the appropriate Lounge Server.

 ****************************************************************************/

#include "pdu/pdu.h"

class CpduRedirectLogin : public CPDU
{
public:
	CpduRedirectLogin();

	int sendRedirectLogin(SOCKET sd,
                        u_int32_t ipaddr,
                        u_int16_t port);
};

#endif