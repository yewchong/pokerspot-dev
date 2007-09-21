#ifndef __network_pdubuyinmore_h__
#define __network_pdubuyinmore_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUBuyInMore
  Created  : 05/11/2000

  OVERVIEW : BuyIn More PDU

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUBuyInMore : public PDUHeader
{
  u_byte_t* write(char* buf) const
  {    
    u_byte_t* pbyte = PDUHeader::write(buf);
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Buy In More PDU ---");
#endif
    return pbyte;
  }

  PDUBuyInMore()
    :
    PDUHeader(PDU_Setup_BuyinMore,
              sizeOf(), NULL, NULL)
  {}
};


END_NAMESPACE(Network)

#endif