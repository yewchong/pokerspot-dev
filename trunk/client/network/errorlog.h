#ifndef __network_errorlog_h__
#define __network_errorlog_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : errorlog.h
  Created  : 08/14/2000

  OVERVIEW : Error logging facility for the client.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

extern void LogError(const char*);

END_NAMESPACE(Network)

#endif