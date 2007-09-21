/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduQueueUpdate
  Created  : 04/18/2000

  OVERVIEW : This PDU is used for broadcasting changes
             in the waiting lists.

 ****************************************************************************/

#include "pduqueueupdate.h"
#include "lounge/player.h"
#include "lounge/game.h"
#include "lounge/waitinglist.h"
#include "tournament/tournament.h"
#include "loadbalancer/loadbalancer.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_Lounge_QueueUpdate, CpduQueueUpdate);


CpduQueueUpdate::CpduQueueUpdate()
{
  type_ = PDU_Lounge_QueueUpdate;
  length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
  stuffHeader();
}

// Send one queue delta
int CpduQueueUpdate::sendQueueDelta(u_int16_t queuenumber,
                                    CPlayer* player,
                                    u_int16_t reason)
{
  length_ += 1 * sizeof(pdu_queuedelta);
  stuffHeader();
  
  char* buf = value_ + PDU_HEADER_SIZE;
  u_int16_t numActions = htons(1);   
  memcpy(buf, &numActions, sizeof(numActions));
  buf += sizeof(numActions);
  
  pdu_queuedelta data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  strncpy(data.name, player->getUsername(), PDU_STRINGSIZE);
  memcpy(buf, &data, sizeof(data));

  if (CLoadBalancer::Inst())
    return sendTo(CLoadBalancer::Inst()->getSocket()); 
  else
    return broadcastToPlayers();
}

int CpduQueueUpdate::sendQueueAdd(u_int16_t queuenumber,
                                  CPlayer* player)
{
  return sendQueueDelta(queuenumber, player, QUEUEDELTA_PLAYER_ADD);
}

int CpduQueueUpdate::sendQueueRemove(u_int16_t queuenumber,
                                     CPlayer* player)
{
  return sendQueueDelta(queuenumber, player, QUEUEDELTA_PLAYER_REMOVE);
}

int CpduQueueUpdate::sendQueueServiced(u_int16_t queuenumber,
                                      CPlayer* player)
{
  return sendQueueDelta(queuenumber, player, QUEUEDELTA_PLAYER_SERVICED);
}

int CpduQueueUpdate::sendFullUpdate(CPlayer* player)
{
  int rc = sendAllTables(player);

  if (rc != -1)
    rc = sendAllPlayers(player);

  return rc;
}

// Send all players in all waiting lists.
int CpduQueueUpdate::sendAllPlayers(CPlayer* player)
{
  char* buf = value_ + PDU_HEADER_SIZE + 2;
  
  u_int16_t numDeltas = 0;
  pdu_queuedelta data;
  memset(&data, 0, sizeof(data));
  
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
      CWaitingList* w = (*wi);
      
      for (PlayerList::iterator pi = w->getPlayers().begin(),
                                pe = w->getPlayers().end();
           pi != pe;
           ++pi)
      {
        if (length_ + (numDeltas + 1) * sizeof(pdu_queuedelta) >= MAXBUFFERSIZE)
        {
          Sys_LogError("CpduQueueUpdate::sendAllPlayers: buffer size too small!\n");
          break;
        }
        
        numDeltas++;
        data.queueNumber = htons(w->getQueueNumber());
        data.reason = htons(QUEUEDELTA_PLAYER_ADD);
        strncpy(data.name, (*pi).player_->getUsername(), PDU_STRINGSIZE);
        memcpy(buf, &data, sizeof(data));
        buf += sizeof(data);
      }
    }
    
    i.next();
  }
  
  length_ = PDU_HEADER_SIZE + 2 + numDeltas * sizeof(pdu_queuedelta);
  stuffHeader();
  
  buf = value_ + PDU_HEADER_SIZE;
  numDeltas = htons(numDeltas);
  memcpy(buf, &numDeltas, sizeof(numDeltas));
  
  return sendTo(player->getSocket());
}

// Send all tables in all waiting lists.
int CpduQueueUpdate::sendAllTables(CPlayer* player)
{
    char* buf = value_ + PDU_HEADER_SIZE + 2;

    u_int16_t numDeltas = 0;

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
            CWaitingList* w = (*wi);

            numDeltas += w->getTableQueueUpdate(length_, buf);
        }

        i.next();
    }

    length_ = PDU_HEADER_SIZE + 2 + numDeltas * sizeof(pdu_queuedelta);
    stuffHeader();

    buf = value_ + PDU_HEADER_SIZE;
    numDeltas = htons(numDeltas);
    memcpy(buf, &numDeltas, sizeof(numDeltas));

    return sendTo(player->getSocket());
}


// Send queue update in list 'deltas' to all players.
int CpduQueueUpdate::broadcastQueueUpdates(const list<pdu_queuedelta>& deltas,
                                           CPlayer* except)
{
    char* buf = (value_ + PDU_HEADER_SIZE + 2);

    u_int16_t numDeltas = deltas.size();
    length_ = PDU_HEADER_SIZE + 2 + numDeltas * sizeof(pdu_queuedelta);
    stuffHeader();

    for (list<pdu_queuedelta>::const_iterator it = deltas.begin(),
                                              end = deltas.end();
         it != end;
         ++it)
    {
        const pdu_queuedelta& delta = *it;

        if (length_ + (numDeltas + 1) * sizeof(pdu_queuedelta) >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduQueueUpdate: buffer size too small!\n");
            break;
        }

        memcpy(buf, &delta, sizeof(delta));
        buf += sizeof(delta);
    }

    buf = value_ + PDU_HEADER_SIZE;
    numDeltas = htons(numDeltas);
    memcpy(buf, &numDeltas, sizeof(numDeltas));

    return broadcastToPlayers(except);
}

int CpduQueueUpdate::sendQueueRemove(u_int16_t queuenumber,
                                     CTable* table)
{
  return sendQueueDelta(queuenumber, table, QUEUEDELTA_TABLE_REMOVE);
}

int CpduQueueUpdate::sendQueueAdd(u_int16_t queuenumber,
                                  CTable* table)
{
  return sendQueueDelta(queuenumber, table, QUEUEDELTA_TABLE_ADD);
}

int CpduQueueUpdate::sendQueueDelta(u_int16_t queuenumber,
                                    CTable* table,
                                    u_int16_t reason)
{
  length_ += 1 * sizeof(pdu_queuedelta);
  stuffHeader();
  
  char* buf = value_ + PDU_HEADER_SIZE;
  u_int16_t numActions = htons(1);   
  memcpy(buf, &numActions, sizeof(numActions));
  buf += sizeof(numActions);
  
  pdu_queuedelta data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  strncpy(data.name, table->getTitle().c_str(), PDU_STRINGSIZE);
  memcpy(buf, &data, sizeof(data));
  
  if (CLoadBalancer::Inst())
    return sendTo(CLoadBalancer::Inst()->getSocket()); 
  else
    return broadcastToPlayers();
}

int CpduQueueUpdate::sendQueueRemove(u_int16_t queuenumber,
                                     CTournament* t)
{
  return sendQueueDelta(queuenumber, t->getTitle(), QUEUEDELTA_TOURNAMENT_REMOVE);
}

int CpduQueueUpdate::sendQueueAdd(u_int16_t queuenumber,
                                  CTournament* t)
{
  return sendQueueDelta(queuenumber, t->getTitle(), QUEUEDELTA_TOURNAMENT_ADD);
}

int CpduQueueUpdate::sendQueueDelta(u_int16_t     queuenumber,
                                    const string& str,
                                    u_int16_t     reason)
{
  length_ += 1 * sizeof(pdu_queuedelta);
  stuffHeader();
  
  char* buf = value_ + PDU_HEADER_SIZE;
  u_int16_t numActions = htons(1);   
  memcpy(buf, &numActions, sizeof(numActions));
  buf += sizeof(numActions);
  
  pdu_queuedelta data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  strncpy(data.name, str.c_str(), PDU_STRINGSIZE);
  memcpy(buf, &data, sizeof(data));

  if (CLoadBalancer::Inst())
    return sendTo(CLoadBalancer::Inst()->getSocket()); 
  else  
    return broadcastToPlayers();
}


void CpduQueueUpdate::grok(SOCKET, CLoadBalancer*)
{
  // Broadcast this PDU to all players.
  broadcastToPlayers();

  
  // This PDU is done
  delete this;
}
