#ifndef __network_pdutournamentdetailsquery_h__
#define __network_pdutournamentdetailsquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUTournamentDetailsQuery
  Created  : 07/07/2000

  OVERVIEW : Lounge Tournament Details Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUTournamentDetailsQuery : public PDUHeader
{
  // Originated from client
  u_int16_t tournament_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUTournamentDetailsQuery(u_int16_t tournament)
    :
    PDUHeader(PDU_Tournament_DetailsQuery,
              sizeOf(), NULL, NULL),
    tournament_(tournament)
  {}

  virtual u_byte_t* write(char* buffer) const
  {
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(tournament_);
    return (u_byte_t*)pint;
  }
};


END_NAMESPACE(Network)

#endif