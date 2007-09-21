#ifndef __network_pdusitinquery_h__
#define __network_pdusitinquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSitInQuery
  Created  : 12.11.1999

  OVERVIEW : Sit In Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSitInQuery : public PDUHeader
{
  void execute(GameState*);

  PDUSitInQuery(const char* username,
                const char* password)
    :
    PDUHeader(PDU_Play_SitInQuery,
              sizeOf(), username, password)
  {}

  PDUSitInQuery(const char* buf)
    :
    PDUHeader(buf)
  {}

  void checkValid()
  {
    PDUHeader::checkValid();
    PDU_DEBUG("--- Recv: SitIn Query PDU ---");
  }
};


END_NAMESPACE(Network)

#endif