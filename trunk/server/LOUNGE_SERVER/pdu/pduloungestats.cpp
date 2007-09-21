/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoungeStats
  Created  : 04/21/2000

  OVERVIEW : Lounge statistics PDU.

             Lounge Server sends this PDU to the Load Balancer
             to update the current statistics of this Lounge.
             Load Balancer computes the grand total and sends
             it to all Lounge Servers.

 ****************************************************************************/

#include "pdu/pduloungestats.h"
#include "loadbalancer/loadbalancer.h"

REGISTER_INCOMING_PDU(PDU_Lounge_LoginStats, CpduLoungeStats);


#pragma NETWORK_ALIGN_BEGIN
struct pdu_stats
{
  u_int32_t players;
  u_int16_t tables;
};
#pragma NETWORK_ALIGN_END


CpduLoungeStats::CpduLoungeStats()
{
  type_ = PDU_Lounge_LoginStats;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_stats);
  stuffHeader();
}

int CpduLoungeStats::sendStats(u_int32_t players,
                               u_int16_t tables)
{
  char* buf = value_ + PDU_HEADER_SIZE;
  pdu_stats pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.players = htonl(players);
  pdu.tables = htons(tables);
  memcpy(buf, &pdu, sizeof(pdu));

  int rc = 0;
  
  if (CLoadBalancer::Inst())
  {
    // Send only to Load Balancer, it will echo it
    // to all other Lounges
    rc = sendToLoadBalancer();
  }
  else
  {
    // Running stand-alone with no Load Balancer - 
    // send stats to players directly
    rc = broadcastToPlayers();
  }

  return rc;
}


void CpduLoungeStats::grok(SOCKET sd, CLoadBalancer*)
{
  // Load Balancer sent this PDU to tell the grand
  // total to all Lounges - forward it to the players.

  OutputDebugString("CpduLoungeStats::grok\n");

  broadcastToPlayers();

  // This PDU is done
  delete this;
}
