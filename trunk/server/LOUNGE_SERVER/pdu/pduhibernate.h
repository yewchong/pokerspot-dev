#ifndef __lounge_pduhibernate_h__
#define __lounge_pduhibernate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduHibernate
  Created  : 10/11/2000

  OVERVIEW : The lounge client uses this pdu to tell the lounge server
             whether the connection is active or inactive. This is used
             to conserve resources with the dual-connection lounge client.             

 ****************************************************************************/

#include "pdu.h"

struct pdu_hibernate
{
    u_int16_t flags;
};


class CpduHibernate : public Cpdu
{
public:
    CpduHibernate();
    void grok(SOCKET sd, CPlayer*);
    u_int16_t getFlags() const;

    enum
    {
      Active = 1,
      InActive = 2
    };
};

inline u_int16_t CpduHibernate::getFlags() const
{
    pdu_hibernate* pdu = (pdu_hibernate*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->flags);
}


#endif