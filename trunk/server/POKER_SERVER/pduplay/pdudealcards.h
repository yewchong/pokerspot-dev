#ifndef __table_pdudealcards_h__
#define __table_pdudealcards_h__

/*******************************************************************************

    NAME:       pduDealCards
    DATE:       4/11/00
    PURPOSE:    Class CpduDealCards implements the send-only PDU type
                PDU_Play_DealCards.  This is the PDU sent to the player to
                inform it of which cards were dealt to him, face down.

                This PDU will need to be tweaked as we move forward with games
                other than Texas Hold'em, because we need to be able to specify
                whether the cards be dealt Face Up, Down, Indian, or Community,
                all in one Deal Cards PDU.


*******************************************************************************/


#include "pduhandler/pduhandler.h"
#include "player/player.h"

typedef struct dealcards_pdu dealcards_pdu_t;

struct dealcards_pdu
{
    u_int16_t dealer;       // specified dealer button position
    u_int16_t card1;        // card value
    u_int16_t suit1;        // card suit
    u_int16_t card2;        // card value
    u_int16_t suit2;        // card suit
    u_int16_t msecs;        // msecs for animation on client
};


class CpduDealCards : public Cpdu
{
public:
    CpduDealCards();
    int sendDealCards(CPlayer *,
                      u_int16_t,
                      u_int16_t,
                      u_int16_t,
                      u_int16_t,
                      u_int16_t,
                      u_int16_t);
};

#endif
