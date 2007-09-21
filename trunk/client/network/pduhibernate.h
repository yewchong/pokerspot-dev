#ifndef __network_pduhibernate_h__
#define __network_pduhibernate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUHibernate
  Created  : 10/11/2000

  OVERVIEW : The lounge client uses this PDU to tell the lounge server
             that the connection is either active or inactive. This is
             used to conserve resources with the dual-connection lounge
             client.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUHibernate : public PDUHeader
{
  u_int16_t flags_;

  enum Action
  {
    Active = 1,
    InActive = 2
  };

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(flags_);
    return (u_byte_t*)pint;
  }

  PDUHibernate(bool active)
    :
    PDUHeader(PDU_Lounge_Hibernate,
              sizeOf(),
              NULL, NULL),
    flags_   (active ? Active : InActive)
  {}

};


END_NAMESPACE(Network)

#endif