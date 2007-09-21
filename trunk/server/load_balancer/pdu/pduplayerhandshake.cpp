/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduHandshake
  Created  : 09/25/2001

  OVERVIEW : This class reads the Load Balancer <-> Player handshake
             data from the network.
             
             NOTE: the socket is in blocking mode.

 ****************************************************************************/

#include "pdu/pduplayerhandshake.h"
#include "common/errorlog.h"
#include "common/sys.h"


CpduPlayerHandshake::CpduPlayerHandshake()
{
  ZeroMemory(sz_username_, sizeof(sz_username_));
}


int CpduPlayerHandshake::readHandshake(SOCKET sd)
{
  // Read the version information:
  // - platform (u_int16_t)
  // - checksum (PDU_CHECKSUMSIZE bytes)
  // - username

  int nbytes = PDU_STRINGSIZE + sizeof(pdu_version);

  int nread = Sys_recv(sd, value_, nbytes);
  if (nread != nbytes)
  {
    Err::Log() << "Can't get player handshake data: "
               << nread << ", " 
               << Sys_GetLastError() << CLog::nl;
    return -1;
  }

  // Copy username to zero-terminated string
  const char* data = value_ + PDU_HEADER_SIZE + sizeof(pdu_version);
  memcpy(sz_username_, data, PDU_STRINGSIZE);
  sz_username_[PDU_STRINGSIZE] = '\0';

  return nbytes;
}