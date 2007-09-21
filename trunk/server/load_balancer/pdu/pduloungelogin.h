#ifndef __lb_pduloungelogin_h__
#define __lb_pduloungelogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogin
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             start-up.

 ****************************************************************************/


#include "pdu/pdu.h"

struct pdu_loungelogin
{
  u_int16_t port;
};


class CpduLoungeLogin : public CPDU
{
public:
  CpduLoungeLogin();
  
  u_int16_t getPort() const;  
  void grok(SOCKET, CLounge*);
};

inline u_int16_t CpduLoungeLogin::getPort() const
{
  pdu_loungelogin* pdu = (pdu_loungelogin*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->port);
}


#endif
