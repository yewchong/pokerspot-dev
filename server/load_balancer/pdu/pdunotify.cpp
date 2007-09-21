/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduNotify
  Created  : 10/05/2001

  OVERVIEW : Send-only Notify pdu.

 ****************************************************************************/

#include "pdu/pdunotify.h"


CpduNotify::CpduNotify()
{
  type_ = PDU_Server_Notify;
  length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
  stuffHeader();
}

int CpduNotify::sendNotifyMessage(SOCKET sd,
                                  const char* msg,
                                  int flags)
{
  char* buf = value_ + PDU_HEADER_SIZE;
  u_int16_t f = htons(flags);
  memcpy(buf, &f, sizeof(f));
  buf += sizeof(f);
  
  if (msg != NULL && *msg != '\0')
  {
    size_t length = strlen(msg);
    memcpy(buf, msg, length);
    buf[length] = '\0';
    length_ += (length + 1);
  }
  
  stuffHeader();
  
  return sendTo(sd);
}

