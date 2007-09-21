#ifndef __network_pduloginstats_h__
#define __network_pduloginstats_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDULoginStats
  Created  : 02/25/2000

  OVERVIEW : Lounge Login Stats PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDULoungeLoginStats : public PDUHeader
{
  // Originated from server
  u_int32_t players_;
  u_int16_t tables_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() + 
                          sizeof(u_int32_t) + 
                          sizeof(u_int16_t); }

  PDULoungeLoginStats(const char* buf)
    :
    PDUHeader(buf),
    players_ (0),
    tables_  (0)
  {
    u_int32_t* pint = (u_int32_t*)&buf[PDU_HEADER_SIZE];
    players_ = ntohl(*pint++);
    u_int16_t* pshort = (u_int16_t*)pint;
    tables_ = ntohs(*pshort++);
  }  

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Login Stats PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("  players: %d tables: %d", players_, tables_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};

END_NAMESPACE(Network)

#endif