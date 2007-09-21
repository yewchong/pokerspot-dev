#ifndef __network_pduhidecards_h__
#define __network_pduhidecards_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUHideCards
  Created  : 01/20/2000

  OVERVIEW : Hide Cards PDU; reply to Show Cards Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUHideCards : public PDUHeader
{
  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
    u_byte_t* p = PDUHeader::write(buffer);
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Hide Cards PDU ---");
#endif
    return p;
  }

  PDUHideCards(const char* username,
               const char* password)
    :
    PDUHeader(PDU_Play_HideCards,
              sizeOf(), username, password)
  {}

  PDUHideCards(const char* buf)
    :
    PDUHeader(buf)
  {
  }
};


END_NAMESPACE(Network)

#endif