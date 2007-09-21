#ifndef __lounge_pduqueueupdate_h__
#define __lounge_pduqueueupdate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduQueueUpdate
  Created  : 04/18/2000

  OVERVIEW : This PDU is used for broadcasting changes
             in the waiting lists.

 ****************************************************************************/


#include "pdu.h"
#include <list>
#include <string>

struct pdu_queuedelta
{
    u_int16_t queueNumber;
    u_int16_t reason;
    char      name[PDU_STRINGSIZE];

    // note: no virtual functions =>
    // sizeof(pdu_queuedelta) == 2 * sizeof(u_int16_t) + PDU_STRINGSIZE

    pdu_queuedelta()
    {
        queueNumber = 0;
        reason = 0;
        memset(name, 0, PDU_STRINGSIZE);
    }
    pdu_queuedelta(const pdu_queuedelta& rhs)
    {
        queueNumber = rhs.queueNumber;
        reason = rhs.reason;
        memcpy(name, rhs.name, PDU_STRINGSIZE);
    }
    pdu_queuedelta& operator = (const pdu_queuedelta& rhs)
    {
        if (this != &rhs)
        {
            queueNumber = rhs.queueNumber;
            reason = rhs.reason;
            memcpy(name, rhs.name, PDU_STRINGSIZE);
        }
        return *this;
    }
};


class CTournament;

class CpduQueueUpdate : public Cpdu
{
public:
  enum {
    QUEUEDELTA_PLAYER_ADD        = 1,
    QUEUEDELTA_PLAYER_REMOVE     = 2,
    QUEUEDELTA_PLAYER_SERVICED   = 3,
    QUEUEDELTA_TABLE_ADD         = 4,
    QUEUEDELTA_TABLE_REMOVE      = 5,
    QUEUEDELTA_TOURNAMENT_ADD    = 6,
    QUEUEDELTA_TOURNAMENT_REMOVE = 7
  };

public:
    CpduQueueUpdate();
    int sendQueueRemove(u_int16_t queuenumber,
                        CPlayer* player);
    int sendQueueAdd(u_int16_t queuenumber,
                     CPlayer* player);
    int sendFullUpdate(CPlayer* player);
    int broadcastQueueUpdates(const list<pdu_queuedelta>& deltas,
                              CPlayer* except);

    int sendQueueRemove(u_int16_t queuenumber,
                        CTable* table);
    int sendQueueAdd(u_int16_t queuenumber,
                     CTable* table);
    int sendQueueRemove(u_int16_t queuenumber,
                        CTournament* tournament);
    int sendQueueAdd(u_int16_t queuenumber,
                     CTournament* tournament);
    int sendQueueServiced(u_int16_t queuenumber,
                          CPlayer*);

    void grok(SOCKET, CLoadBalancer*);

private:
    int sendQueueDelta(u_int16_t queuenumber,
                       CPlayer* player,
                       u_int16_t reason);
    int sendQueueDelta(u_int16_t queuenumber,
                       CTable* table,
                       u_int16_t reason);
    int sendAllPlayers(CPlayer* player);
    int sendAllTables(CPlayer* player);

    int sendQueueDelta(u_int16_t     queuenumber,
                       const string& str,
                       u_int16_t     reason);
};

#endif