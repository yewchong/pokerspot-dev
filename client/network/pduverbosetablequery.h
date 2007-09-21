#ifndef __network_pduverbosetablequery_h__
#define __network_pduverbosetablequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUVerboseTableQuery
  Created  : 02/25/2000

  OVERVIEW : Lounge Verbose Table Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUVerboseTableQuery : public PDUHeader
{
  // Originated from client
  u_int16_t table_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUVerboseTableQuery(const char* username,
                       const char* password,
                       u_int16_t   tablenum)
    :
    PDUHeader(PDU_Lounge_VerboseTableQuery,
              sizeOf(), username, password),
    table_   (tablenum)
  {}

  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PDU_DEBUG_
    PDU_DEBUG("--- Send: Verbose Table Query PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(table_);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   table: %d ", table_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }
};


END_NAMESPACE(Network)

#endif