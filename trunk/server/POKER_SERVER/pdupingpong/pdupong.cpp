#include "pdupong.h"
#include "pduping.h"
#include "../table/table.h"
#include "../pdunotify/pdunotify.h"
#include "../pduplayer/pduplayer.h"
#include "../sys.h"
#include "tournament/tournament.h"


REGISTER_INCOMING_PDU(PDU_Pong, CpduPong);

CpduPong::CpduPong()
{}

void CpduPong::grok(SOCKET sd, CTable* table)
{
    SingleLock l(&table->getTableMutex());
    l.lock();

    CPlayer* player = table->getPlayerRawSocket(sd);
    if (player)
    {
        pdu_ping* ping = (pdu_ping*)(value_ + PDU_HEADER_SIZE);
        u_int32_t sent = ntohl(ping->data);

        long now = 0;
        time(&now);
        if (now >= (long)sent &&
            now - (long)sent < PING_ACCEPT_THRESHOLD)
        {
            if (!CTournament::Inst()->isTournament())
            {
              //
              // Player replied to ping in time - he
              // can play next hand
              //
              if (player->matchState(PLAYER_STATE_ALLIN) ||
                  player->matchState(PLAYER_STATE_NOTRESPONDING))
              {
                  player->stateChange(PLAYER_STATE_SITOUT);
  
                  CpduPlayer pdu(table);
                  pdu.sendPlayerRaw(player, PLAYER_SITOUT);
              }
              player->setPings(0);
            }

            char buf[200];
            sprintf(buf, "Player %s socket %d ping reply ok\n",
                    player->getUsername(), player->getConnfd());
            Sys_LogError(buf);
        }
        else
        {
            char buf[200];
            sprintf(buf, "Player %s socket %d ping reply NOT ok\n",
                    player->getUsername(), player->getConnfd());
            Sys_LogError(buf);
        }
    }

    // this pdu is done
    delete this;
}
