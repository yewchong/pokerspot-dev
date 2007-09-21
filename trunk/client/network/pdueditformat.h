#ifndef __network_pdueditformat_h__
#define __network_pdueditformat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUEditRules
  Created  : 08/25/2000

  OVERVIEW : 

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUEditFormat : public PDUHeader
{
  PDUEditFormat(const char* buf);
  void execute(GameState*);

  PDUEditFormat()
    :
    PDUHeader(PDU_Tournament_EditFormat,
              sizeOf(), NULL, NULL)
  {}
};


END_NAMESPACE(Network)

#endif