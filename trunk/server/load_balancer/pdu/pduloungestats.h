#ifndef __lb_pduloungestats_h__
#define __lb_pduloungestats_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeStats
  Created  : 10/01/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer
             to inform it of new player/table counts.

 ****************************************************************************/


#include "pdu/pdu.h"
#include "common/networkalign.h"


#pragma NETWORK_ALIGN_BEGIN
struct pdu_stats
{
  u_int32_t players;
  u_int16_t tables;
};
#pragma NETWORK_ALIGN_END


class CpduLoungeStats : public CPDU
{
public:
  CpduLoungeStats();
  
  u_int32_t getNumPlayers() const;
  u_int16_t getNumTables() const;
  void grok(SOCKET, CLounge*);
};

inline u_int32_t CpduLoungeStats::getNumPlayers() const
{
  pdu_stats* pdu = (pdu_stats*)(value_ + PDU_HEADER_SIZE);
  return ntohl(pdu->players);
}

inline u_int16_t CpduLoungeStats::getNumTables() const
{
  pdu_stats* pdu = (pdu_stats*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->tables);
}



#endif
