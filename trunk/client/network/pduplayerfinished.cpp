
/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUTournamentPlayerFinished
  Created  : 08/22/2000

  OVERVIEW : PDU used for notifying clients that a player
             was eliminated from the tournament.

 ****************************************************************************/


#include "stdafx.h"
#include "network/pduplayerfinished.h"
#include "ui/chatview.h"

BEGIN_NAMESPACE(Network)

const char* GetOrd(u_int16_t pos)
{
    static char buf[100];

    if (pos == 11)
    {
        sprintf(buf, "11th");
    }
    else if (pos == 12)
    {
        sprintf(buf, "12th");
    }
    else if (pos == 13)
    {
        sprintf(buf, "13th");
    }
    else
    {
        int tens = (pos / 10);
        int ones = pos - (10 * tens);

        switch (ones)
        {
        case 1:
            if (tens)
                sprintf(buf, "%i%s", tens, "1st");
            else
                sprintf(buf, "%s", "1st");
            break;
    
        case 2:
            if (tens)
                sprintf(buf, "%i%s", tens, "2nd");
            else
                sprintf(buf, "%s", "2nd");
            break;
        case 3:
            if (tens)
                sprintf(buf, "%i%s", tens, "3rd");
            else
                sprintf(buf, "%s", "3rd");
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 0:
        default:
            sprintf(buf, "%ith", pos);
            break;
        }
    }
    
    return buf;
}

void PDUTournamentPlayerFinished::execute(GameState*)
{
#ifdef PSPOT_TABLE_MODULE_

    const char* ord = GetOrd(pos_);
    CString s;
    if (table_ != 0xffff)
    {       
        s.Format("%s (table %d) finishes in %s place",
                 username_, table_, ord);
    }
    else
    {
        s.Format("%s finishes in %s place",
                 username_, ord);
    }

    if (CChatView::Instance())
        CChatView::Instance()->addDealerMessage(s, CChatView::CF_GameNormal);

#endif
}

END_NAMESPACE(Network)
