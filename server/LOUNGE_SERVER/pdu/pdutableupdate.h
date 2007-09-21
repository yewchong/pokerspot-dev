#ifndef __lounge_pdutableupdate_h__
#define __lounge_pdutableupdate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableUpdate
  Created  : 04/24/2000

  OVERVIEW : Receive-only Table Update PDU - table server sends
             this pdu to inform lounge server about state change.

 ****************************************************************************/

#include "pdu.h"
    

struct pdu_tableupdate
{
    u_int16_t numplayers;
};

class CpduTableUpdate : public Cpdu
{
public:
    CpduTableUpdate();
    void grok(SOCKET sd, CTable*);

    u_int16_t getNumPlayers() const;
};

inline u_int16_t CpduTableUpdate::getNumPlayers() const
{
    pdu_tableupdate* pdu = (pdu_tableupdate*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->numplayers);
}


#endif