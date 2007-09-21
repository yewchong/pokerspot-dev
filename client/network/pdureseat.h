#ifndef __network_pdureseat_h__
#define __network_pdureseat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUReseat
  Created  : 07/26/2000

  OVERVIEW : Reseat PDU. Sent by the table server when
             the table is dissolved and the player is reseated
             at another table.

 ****************************************************************************/

#include "network/network.h"

BEGIN_NAMESPACE(Network)

struct PDUReseat : public PDUHeader
{
  // Originated from server
  u_int32_t host_;
  u_int16_t port_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int32_t) +
                                 sizeof(u_int16_t); }

  PDUReseat(const char* buf)
    :
    PDUHeader(buf),
    host_(0),
    port_(0)
  {
    u_int32_t* plong = (u_int32_t*)&buf[PDU_HEADER_SIZE];
    host_ = ntohl(*plong++);
    u_int16_t* pint = (u_int16_t*)plong;
    port_ = ntohs(*pint++);
  }

};


END_NAMESPACE(Network)


#endif