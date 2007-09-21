/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduStatus
  Created  : 04/18/2000

  OVERVIEW : Lounge status PDU.

 ****************************************************************************/

#include "pdustatus.h"
#include "lounge/game.h"
#include "lounge/waitinglist.h"
#include "common/chips.h"
#include "sys.h"

struct pdu_status
{
    u_int16_t queuenum;
    u_int16_t queuesize;
    CChips::chip_amount_t tablelow;
    CChips::chip_amount_t tablehigh;
    u_int16_t tabletype;
    u_int16_t tablequeue;
};


// Put string length and the string to buffer.
int SerializeString(char* buf, const char* s)
{
    u_int16_t length = 0;
    if (s && *s != '\0')
        length = strlen(s);
    length = htons(length);
    memcpy(buf, &length, sizeof(length));
    length = ntohs(length);
    buf += sizeof(length);
    memcpy(buf, s, length);
    return (sizeof(length) + length);
}

CpduStatus::CpduStatus()
{
    type_ = PDU_Lounge_Status;
    length_ = PDU_HEADER_SIZE;

    char* buf = value_ + PDU_HEADER_SIZE;
        
    GameIterator i = CGame::GetGames();
    while (i.more())
    {
        CGame& game = i.current();

        WaitingLists& wl = game.getWaitingLists();
        for (WaitingLists::iterator wi = wl.begin(),
                                    we = wl.end();
             wi != we;
             ++wi)
        {
            if (length_ + sizeof(pdu_status) >= MAXBUFFERSIZE)
            {
                Sys_LogError("CpduStatus: buffer size too small!\n");
                break;
            }

            CWaitingList* w = (*wi);
            pdu_status pdu;
            pdu.queuenum = htons(w->getQueueNumber());
            pdu.queuesize = htons(0);
            pdu.tablelow = w->getLo().htonl_rep();
            pdu.tablehigh = w->getHi().htonl_rep();
            pdu.tabletype = htons(game.getGameSubType());
            pdu.tablequeue = htons(0);
            
            memcpy(buf, &pdu, sizeof(pdu));
            buf += sizeof(pdu);
            length_ += sizeof(pdu);
        }

        i.next();
    }

    stuffHeader();
}

int CpduStatus::sendGameNames(SOCKET sd)
{
    type_ = PDU_Lounge_GameNames;
    length_ = PDU_HEADER_SIZE;

    char* buf = value_ + PDU_HEADER_SIZE;

    // Copy number of games to buffer
    u_int16_t numGames = CGame::GetNumGames();
    u_int16_t* pint = (u_int16_t*)buf;
    *pint++ = htons(numGames);
    length_ += sizeof(numGames);
    
    // Copy game name strings to buffer
    GameIterator i = CGame::GetGames();
    while (i.more())
    {
        CGame& game = i.current();

        // Put game type to buffer
        u_int16_t gameType = game.getGameSubType();
        *pint++ = htons(gameType);
        buf = (char*)pint;

        // Put game name length & string to buffer
        const char* n = game.getGameName();

        int l = n ? strlen(n) : 0;
        if (length_ + l >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduStatus: buffer size too small for game names!\n");
            break;
        }

        int nameLength = SerializeString(buf, n);
        buf += nameLength;
        length_ += (sizeof(gameType) + nameLength);

        i.next();
        pint = (u_int16_t*)buf;
    }

    stuffHeader();

    return sendTo(sd);
}