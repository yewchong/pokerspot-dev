#ifndef __network_pduupgraderequest_h__
#define __network_pduupgraderequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUUpgradeRequest
  Created  : 01/30/2000

  OVERVIEW : Upgrade Request PDU.

             When the lounge server detects that the
             client has old versions of files, it sends
             an Upgrade Request PDU that contains the new
             client configuration file.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUUpgradeRequest : public PDUHeader
{
  // Originated from server

  u_int32_t numBytes_;
  message_t config_data_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int32_t); }

  PDUUpgradeRequest(const char* buf)
    :
    PDUHeader   (buf),
    numBytes_   (0),
    config_data_(0)
  {
    u_int32_t* pint = (u_int32_t*)&buf[PDU_HEADER_SIZE];
    numBytes_ = htonl(*pint++);
    u_byte_t* pbyte = (u_byte_t*)pint;

    if (numBytes_ > 0)
    {
      config_data_ = new char[numBytes_];
      if (config_data_)
      {
        memcpy(config_data_, (char*)pbyte, numBytes_);
      }
    }
  }

  ~PDUUpgradeRequest()
  {
    delete [] config_data_;
    config_data_ = NULL;
  }
    
  void checkValid()
  {
    PDU_DEBUG("--- Recv: Upgrade Request PDU ---");
    PDUHeader::checkValid();
  }
};


END_NAMESPACE(Network)

#endif