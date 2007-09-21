#ifndef __table_pduallin_h__
#define __table_pduallin_h__

/*******************************************************************************

    NAME:       pduAllIn
    DATE:       4/12/00
    PURPOSE:    Class CpduAllIn implements the send-only PDU type
                PDU_Play_AllIn.  This is the PDU sent to all players
                to inform them that a player has gone "all-in".

*******************************************************************************/


#include "pduhandler/pduhandler.h"
#include "player/player.h"
#include "table/table.h"
#include "common/chips.h"

#pragma NETWORK_ALIGN_BEGIN
struct allin_pdu
{
  u_int16_t slot;               // slot of the player going all-in
  CChips::chip_amount_t topay;  // how much all-in player should pay
};
#pragma NETWORK_ALIGN_END


class CpduAllIn : public Cpdu
{
public:
  CpduAllIn(CTable*);
  int sendAllIn(CPlayer*, const CChips& toPay);
    

private:
  CTable* table_;
};


#endif
