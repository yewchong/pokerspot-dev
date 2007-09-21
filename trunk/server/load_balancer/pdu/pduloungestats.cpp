/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeStats
  Created  : 10/01/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer
             to inform it of new player/table counts.

 ****************************************************************************/


#include "pdu/pduloungestats.h"
#include "loadbalancer/lounge.h"
#include "loadbalancer/loadbalancer.h"


REGISTER_INCOMING_PDU(PDU_Lounge_LoginStats, CpduLoungeStats);


CpduLoungeStats::CpduLoungeStats()
{}


void CpduLoungeStats::grok(SOCKET, CLounge* lounge)
{
  // update stats

  if (lounge)
  {
    lounge->setNumPlayers(getNumPlayers());
    lounge->setNumTables(getNumTables());
  }

  // Compute grand total
  int numTables = 0, numPlayers = 0;

  for (LoungeIterator li = CLB::Inst()->getLounges(); 
       li.more(); li.next())
  {
    CLounge* lounge = li.current();

    numTables += lounge->getNumTables();
    numPlayers += lounge->getNumPlayers();
  }

  pdu_stats data;
  ZeroMemory(&data, sizeof(data));
  data.players = htonl(numPlayers);
  data.tables = htons(numTables);

  memcpy(value_ + PDU_HEADERSIZE, &data, sizeof(data));

  broadcast(INVALID_SOCKET);


  // this pdu is done
  delete this;
}

