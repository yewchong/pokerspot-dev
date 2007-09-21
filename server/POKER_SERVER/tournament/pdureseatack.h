#ifndef __table_pdureseatack_h__
#define __table_pdureseatack_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatAck
  Created  : 07/25/2000

  OVERVIEW : Send-only reseat ack pdu.
             Table server tells lounge server whether seating a player
             was successfull or not.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

struct pdu_reseatack
{
    char username[PDU_STRINGSIZE];
};

class CpduReseatAck : public Cpdu
{
public:
    CpduReseatAck();
    int sendAck  (SOCKET      sd,
                  const char* username);
    int sendNack (SOCKET      sd,
                  const char* username);
};


#endif