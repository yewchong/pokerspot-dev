#ifndef __network_pduloungebuyinresponse_h__
#define __network_pduloungebuyinresponse_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeBuyInReply
  Created  : 03/13/2000

  OVERVIEW : Lounge Buy IN Reply PDU

 ****************************************************************************/

#include "network/network.h"
#include "network/pduaction.h"

BEGIN_NAMESPACE(Network)


struct PDULoungeBuyInReply : public PDUHeader
{
  // Originated from server
  u_int16_t indicator_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDULoungeBuyInReply(const char* buf)
    :
    PDUHeader (buf),
    indicator_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    indicator_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Buy In Reply PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   indicator: %d", indicator_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }

};


END_NAMESPACE(Network)


#endif