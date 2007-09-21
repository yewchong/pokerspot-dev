#ifndef __network_moduleversion_h__
#define __network_moduleversion_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUModuleVersion
  Created  : 01/28/2000

  OVERVIEW : Module Version PDU. This PDU is not a PDU type,
             it is data only (sent along with the login PDU).

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


#define MODULE_SIZE 16
#define CHECKSUM_SIZE 16

struct PDUModuleVersion
{
  u_byte_t module_[MODULE_SIZE];
  u_byte_t checksum_[CHECKSUM_SIZE];

  size_t sizeOf() const { return sizeof(module_) +
                                 sizeof(checksum_); }

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: module version data ---");
    PDU_DEBUG((char*)module_);
    memcpy(buf, module_, sizeof(module_));
    buf += sizeof(module_);
    memcpy(buf, checksum_, sizeof(checksum_));
    return (u_byte_t*)buf;
  }

  PDUModuleVersion()
  {}

};


END_NAMESPACE(Network)

#endif