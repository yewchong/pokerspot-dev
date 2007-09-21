/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Networkbuffer
  Created  : 24.12.1999

  OVERVIEW : Class for serializing data to/from a network-compatible buffer.

 ****************************************************************************/

#include "stdafx.h"
#include "network/networkbuffer.h"

BEGIN_NAMESPACE(Network)


WriteBuffer::WriteBuffer(char* buf)
  :
  buf_((u_byte_t*)buf),
  pos_((u_byte_t*)buf)
{}


ReadBuffer::ReadBuffer(const char* buf)
  :
  buf_((const u_byte_t*)buf),
  pos_((const u_byte_t*)buf)
{}


END_NAMESPACE(Network)
