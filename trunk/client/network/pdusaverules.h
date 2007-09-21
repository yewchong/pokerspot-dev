#ifndef __network_pdusaverules_h__
#define __network_pdusaverules_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSaveRules
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSaveRules : public PDUHeader
{
  u_int16_t flags_;
  char*     data_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(flags_);

    u_byte_t* pbyte = (u_byte_t*)pint;
    if (data_)
    {
        int len = strlen(data_);
        memcpy(pbyte, data_, len);
        pbyte[len + 1] = '\0';
        pbyte += (len + 1);
    }

    return pbyte;
  }

  int sendChunk(SOCKET);

  PDUSaveRules(bool, const char* buf);
  ~PDUSaveRules();
};


END_NAMESPACE(Network)

#endif