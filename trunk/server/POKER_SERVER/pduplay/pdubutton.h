#ifndef __table_pdubutton_h__
#define __table_pdubutton_h__

/*******************************************************************************

    NAME:       pduButton
    DATE:       4/11/00
    PURPOSE:    Class CpduButton implements the send-only PDU type
                PDU_Play_Button.  This is the PDU sent to the player to
                inform it as to which player is the "dealer".

*******************************************************************************/


#include "pduhandler/pduhandler.h"
#include "player/player.h"
#include "table/table.h"

struct button_pdu
{
    u_int16_t slot; // which slot is currently the dealer (button-holder)
};

class CpduButton : public Cpdu
{
public:
    CpduButton(CTable *);
    int sendButton(void);
private:
    CTable *table_;
};

#endif 