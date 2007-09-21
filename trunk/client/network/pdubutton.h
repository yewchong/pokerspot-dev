#ifndef __network_pdubutton_h__
#define __network_pdubutton_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUButton
  Created  : 01/05/2000

  OVERVIEW : Button PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUButton : public PDUHeader
{
  u_int16_t dealerslot_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(dealerslot_);
    return (u_byte_t*)pint;
  }

  PDUButton(const char* username,
            const char* password,
            u_int16_t   dealerslot)
    :
    PDUHeader   (PDU_Play_Button, sizeOf(),
                 username, password),
    dealerslot_ (dealerslot)
  {
    assert(dealerslot_ <= 9);
  }

  PDUButton(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    dealerslot_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Button PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(dealerslot_ < 10);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   dealerslot: %d", dealerslot_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif