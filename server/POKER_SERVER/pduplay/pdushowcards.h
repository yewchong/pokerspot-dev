#ifndef __table_showcards_h__
#define __table_showcards_h__

/*******************************************************************************

    NAME:       pduShowCards
    DATE:       4/16/00
    PURPOSE:    Receive-only ShowCards pdu

*******************************************************************************/


#include "../pduhandler/pduhandler.h"

struct pdu_showcards
{
    u_int16_t showCards;
};

class CpduShowCards : public Cpdu
{
public:
    CpduShowCards();
    void grok(SOCKET, CTable*);
    bool getShowCards() const;
};

inline bool CpduShowCards::getShowCards() const
{
    pdu_showcards* pdu = (pdu_showcards*)(value_ + PDU_HEADER_SIZE);
	return (ntohs(pdu->showCards) == 1);
}



#endif