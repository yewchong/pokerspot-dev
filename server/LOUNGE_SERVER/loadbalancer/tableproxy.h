#ifndef __lounge_tableproxy_h__
#define __lounge_tableproxy_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : Class CTableProxy
  Created  : 10/09/2001

  OVERVIEW : This CTable subclass is used as a proxy to represent
             a table logged in to another Lounge Server.

 ****************************************************************************/


#include "lounge/table.h"
#include <list>

class CTableProxy : public CTable
{
public:
  CTableProxy(int number,
              u_long ipaddress,
              int port);

  // CTable overrides
  int send(Cpdu& pdu);
};


typedef list<CTableProxy> RemoteTables;

#endif
