#ifndef __network_pdubroadtablequery_h__
#define __network_pdubroadtablequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUBroadTableQuery
  Created  : 02/25/2000

  OVERVIEW : Lounge Broad Table Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUBroadTableQuery : public PDUHeader
{
  // Originated from client
  u_int16_t table_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }


  PDUBroadTableQuery(const char* username,
                     const char* password,
                     u_int16_t   tablenum)
    :
    PDUHeader(PDU_Lounge_BroadTableQuery,
              sizeOf(), username, password),
    table_   (tablenum)
  {}

  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Broad Table Query PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(table_);
  }
};


END_NAMESPACE(Network)

#endif