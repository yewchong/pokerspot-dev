#ifndef __table_pdudealcardsflex_h__
#define __table_pdudealcardsflex_h__

/*******************************************************************************

    NAME:       pduDealCardsFlex
    DATE:       4/25/00
    PURPOSE:    Class CpduDealCardsFlex implements the send-only PDU type
                PDU_Play_DealCardsFlex.  This is the PDU sent to the player to
                inform it of which cards were dealt to him, face down.

                This is the new "tweaked" DealCards PDU.  It affords us
                extended flexibility in the dealcards function.  Now we
                can specify Community, Down, Up, Indian, etc.


*******************************************************************************/


#include "pduhandler/pduhandler.h"
#include "player/player.h"

typedef struct dealcardsflex_pdu dealcardsflex_pdu_t;

enum DealCardsType
{
    Hole = 1,
    Community = 2,
    Indian = 3,
    Dummy = 4
};

struct dealcardsflex_pdu
{
    u_int16_t slot;
    u_int16_t type;
    u_int16_t numcards;
};

struct dealcardsflex_cardinfo
{
    u_int16_t suit;
    u_int16_t value;
};


class CpduDealCardsFlex : public Cpdu
{
public:
    CpduDealCardsFlex();
    void setDeal(u_int16_t, DealCardsType);
    void addCard(u_int16_t, u_int16_t);
    int sendDealCards(CPlayer *);
    int sendDummyDealCards(CPlayer *, int numCards);
    int sendAllDealCards(Cring*); // XXX 7-stud
};

#endif
