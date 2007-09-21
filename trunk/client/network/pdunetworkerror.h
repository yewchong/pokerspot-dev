#ifndef __network_pdunetworkerror_h__
#define __network_pdunetworkerror_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUNetworkError
  Created  : 16.11.1999

  OVERVIEW : Network Error PDU.
             This PDU is sent by the client itself after a
             fatal network error (i.e., the connection to the
             server was terminated. The action taken here is
             to shut down the client's networking and display
             an error message to the user.

 ****************************************************************************/

#include "network/network.h"

BEGIN_NAMESPACE(Network)

struct PDUNetworkError : public PDUHeader
{
  int errorCode_;

  void execute(GameState*);

  PDUNetworkError(int errCode)
    :
    PDUHeader(PDU_Error_Generic,
              sizeOf(),
              "x", "x"),
    errorCode_(errCode)
  {}

  PDUNetworkError(const PDUNetworkError& p)
    :
    PDUHeader(p),
    errorCode_(p.errorCode_)
  {}
};


END_NAMESPACE(Network)

#endif