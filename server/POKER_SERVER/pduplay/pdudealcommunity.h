#ifndef __table_pdudealcommunity_h__
#define __table_pdudealcommunity_h__

/*******************************************************************************

    NAME:       pduDealCommunity
    DATE:       4/11/00
    PURPOSE:    Class CpduDealCommunity implements the send-only PDU type
                PDU_Play_DealCommunity.  This is the PDU sent to the player to
                inform it of which cards were dealt to the board, face up.

                This PDU will need to be merged into the main pduDealCards
                pdu as we move forward, for obvious reasons.

*******************************************************************************/

#include "pduhandler/pduhandler.h"
#include "player/player.h"
#include "ring/ring.h"

typedef struct dealcommunity_pdu dealcommunity_pdu_t;

struct dealcommunity_pdu
{
    u_int16_t value;        // card value
    u_int16_t suit;         // card suit
    u_int16_t msecs;        // msecs for client animation
};


class CpduDealCommunity : public Cpdu
{
public:
    CpduDealCommunity();
    int sendDealCommunity(Cring *, u_int16_t, u_int16_t, u_int16_t);
private:
    CTable *table_;

};

#endif 

