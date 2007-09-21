/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CPDULoungeHandshake
  Created  : 09/30/2001

  OVERVIEW : This PDU does the initial handshake with the
             Load Balancer server.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduloungehandshake.h"


BEGIN_NAMESPACE(Network)


u_byte_t* CPDULoungeHandshake::write(char* buffer) const
{
  PDU_DEBUG("--- Send: Lounge Handshake PDU ---");
  
  u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);

  // Write platform and checksum
  *pint++ = htons(platform_);
  u_byte_t* pbyte = (u_byte_t*)pint;
  memcpy(pbyte, checksum_, CLIENT_CHECKSUM_SIZE);
  pbyte += CLIENT_CHECKSUM_SIZE;

  // Write username
  memcpy(pbyte, username_, PDU_STRINGSIZE);
  pbyte += PDU_STRINGSIZE;

  return pbyte;
}


CPDULoungeHandshake::CPDULoungeHandshake(
                       const char* username,
                       u_int16_t   platform,
                       u_byte_t*   checksum)
  :
  PDUHeader(PDU_LB_Handshake, sizeOf(), NULL, NULL),
  platform_(platform)
{
  if (username != NULL && *username != '\0')
    strncpy(username_, username, PDU_STRINGSIZE);
  else
    memset(username_, 0, sizeof(username_));
  
  memcpy(checksum_, checksum, CLIENT_CHECKSUM_SIZE);
}


END_NAMESPACE(Network)