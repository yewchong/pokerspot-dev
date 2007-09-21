#ifndef __network_pdueditrules_h__
#define __network_pdueditrules_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUEditRules
  Created  : 08/25/2000

  OVERVIEW : 

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUEditRules : public PDUHeader
{
  u_int16_t flags_;
  char*     data_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUEditRules(const char* buf);
  PDUEditRules(bool)
    :
    PDUHeader(PDU_Tournament_EditFormat,
              sizeOf(), NULL, NULL),
    flags_(0),
    data_ (0)
  {}
  PDUEditRules()
    :
    PDUHeader(PDU_Tournament_EditRules,
              sizeOf(), NULL, NULL),
    flags_(0),
    data_ (0)
  {}
  ~PDUEditRules();
};


END_NAMESPACE(Network)

#endif