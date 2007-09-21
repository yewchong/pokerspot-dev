#ifndef __network_pdushowcardsquery_h__
#define __network_pdushowcardsquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUShowCards
  Created  : 01/20/2000

  OVERVIEW : Show Cards Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUShowCardsQuery : public PDUHeader
{
  // Originated by server
  void execute(GameState*);

  PDUShowCardsQuery(const char* username,
                    const char* password)
    :
    PDUHeader(PDU_Play_ShowCardsQuery,
              sizeOf(), username, password)
  {}

  PDUShowCardsQuery(const char* buf)
    :
    PDUHeader(buf)
  {
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Show Cards Query PDU ---");
    PDUHeader::checkValid();
  }
};


END_NAMESPACE(Network)

#endif