/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUHandLogRequest
  Created  : 03/08/2000

  OVERVIEW : Hand Log Request PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduhandlogrequest.h"


BEGIN_NAMESPACE(Network)

PDUHandLogRequest::PDUHandLogRequest(u_int16_t   num_hands,
                                     const char* emailaddr,
                                     int         addrlen)
  :
  PDUHeader(PDU_Setup_HandLogRequest,
            sizeOf() + addrlen + 1,
            NULL, NULL),
  num_hands_(num_hands),
  addrlen_  (addrlen),
  emailaddr_(0)
{
  emailaddr_ = new char[addrlen + 1];
  if (emailaddr_)
  {
    memcpy(emailaddr_, emailaddr, addrlen);
    emailaddr_[addrlen] = '\0';
  }
}


u_byte_t* PDUHandLogRequest::write(char* buffer) const
{
#ifdef PCLIENT_MODULE_
  PDU_DEBUG("--- Send: Hand Log Request PDU ---");
  {
    CString s;
    PDU_ASSERT(emailaddr_ != 0)
    if (emailaddr_ != 0)
    {
      s.Format("   num_hands: email: %d: %s", num_hands_, emailaddr_);
      PDU_DEBUG((LPCSTR)s); 
    }
  }
#endif
  u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);

  // Write number of hands requested
  *pint++ = htons(num_hands_);

  // Write length of address
  *pint++ = htons(addrlen_);
  u_byte_t* pbyte = (u_byte_t*)pint;

  // Write variable length email address
  size_t msglen = strlen(emailaddr_);
  size_t len = length_ - sizeOf();
  assert(msglen == (len - 1));
  memcpy(pbyte, emailaddr_, msglen);
  pbyte += msglen;
  *pbyte++ = '\0';

  return pbyte;
}


END_NAMESPACE(Network)
