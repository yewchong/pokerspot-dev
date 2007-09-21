#ifndef __network_pducreditcardtypes_h__
#define __network_pducreditcardtypes_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCreditCardTypes
  Created  : 04/24/2001

  OVERVIEW : Credit Card Types PDU; sent by the lounge server
             to the client to enumerate supported credit card types.

 ****************************************************************************/

#include "compat.h"
#include "network/network.h"
#include <list>
#include <string>


BEGIN_NAMESPACE(Network)


struct PDUCreditCardTypes : public PDUHeader
{
  // Originated by server

  u_int16_t num_types_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUCreditCardTypes(const char* buf);

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Credit Card Types PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   numcards: %d", num_types_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }

private:
  std::list<std::string> cctypes_;
};


END_NAMESPACE(Network)

#endif