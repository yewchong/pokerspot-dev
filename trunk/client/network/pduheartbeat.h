#ifndef __network_pduheartbeat_h__
#define __network_pduheartbeat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUHeartBeat
  Created  : 08/26/2000

  OVERVIEW : Heart beat pdu. Client sends periodically
             this pdu in order to detect dead connection.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUHeartBeat : public PDUHeader
{
  PDUHeartBeat()
    :
    PDUHeader(PDU_HeartBeat,
              sizeOf(), NULL, NULL)
  {}
};


END_NAMESPACE(Network)

#endif