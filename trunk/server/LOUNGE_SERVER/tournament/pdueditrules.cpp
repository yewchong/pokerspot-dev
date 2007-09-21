/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduEditRules
  Created  : 08/25/2000

  OVERVIEW : 

 ****************************************************************************/

#include "pdueditrules.h"
#include "lounge/player.h"
#include "tournament/tournamentqueue.h"
#include "tournament/tournament.h"
#include "sys.h"

/* NOT IMPLEMENTED

REGISTER_INCOMING_PDU(PDU_Tournament_EditRules, CpduEditRules);

CpduEditRules::CpduEditRules()
{
    type_ = PDU_Tournament_EditRules;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

void CpduEditRules::grok(SOCKET, CPlayer* player)
{
    // Load the rules file
    const char* data = NULL;

    CpduEditRules pdu;
    pdu.sendRules(player->getSocket());
}

struct pdu_data
{
    u_int16_t flags;
};


int CpduEditRules::sendRules(SOCKET sd)
{
    type_ = PDU_Tournament_EditRules;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();

    // Load the tournament definition file into a buffer
    long size = 0, osize = 0;

    FILE* fp = fopen("tournament.tdl", "rb");
    if (!fp)
    {
        Sys_LogError("CpduEditRules: Can't open 'tournament.tdl'");
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) == 0)
    {
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }

    // Can send at max USHRT_MAX bytes of data
    if (size >= USHRT_MAX)
    {
        Sys_LogError("CpduEditRules: size >=USHRT_MAX");
        return 0;
    }

    char* buf = new char[size];
    if (buf == NULL)
    {
        Sys_LogError("CpduEditRules: Out of memory");
        return -1;
    }

    //
    // Then stuff the file in to a buffer 
    //

    long ofs = 0;
    osize = size;

    while (size > 0)
    {
        long nread = fread(&buf[ofs], 1, 4096, fp);
            
        if (nread == 0)
            break;

        ofs += nread;
        size -= nread;
    }

    fclose(fp);
    fp = NULL;

    // Send the data
    sendData(sd, buf, osize);

    delete [] buf;
    buf = NULL;

    return 0;
}

int CpduEditRules::sendFormat(SOCKET sd)
{
    type_ = PDU_Tournament_EditRules;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();

    CTournamentQueue* tq = CTournamentQueue::GetQueue();
    if (tq)
    {
        CTournament* t = tq->findTournament(1);
        if (t)
        {
            string descr = t->getDescription();

            // Send the data
            sendData(sd, descr.c_str(), descr.size());
        }
    }

    return 0;
}

int CpduEditRules::sendData(SOCKET      sd,
                            const char* data,
                            int         nbytes)
{
    length_ += sizeof(pdu_data) + nbytes;
    if (length_ > MAXBUFFERSIZE)
    {
        Sys_LogError("CpduEditRules: buffer size too small!");
        return -1;
    }
    stuffHeader();

    pdu_data pdu;
    pdu.flags = 1;
    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
    memcpy(value_ + PDU_HEADER_SIZE + sizeof(pdu), data, nbytes);

    return sendTo(sd);
}

*/