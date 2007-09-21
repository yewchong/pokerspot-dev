#ifndef __network_pduchatenable_h__
#define __network_pduchatenable_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class PDUChatEnable
  Created  : 05/18/2001

  OVERVIEW : Chat enable/disable pdu.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUEnableChat : public PDUHeader
{
  enum EnableChatFlag
  {
    EC_TournamentSeating = 1,
    EC_Bandwidth         = 2,
    EC_YourChatDisabled  = 4,
    EC_ChatDisabled      = 8,
    EC_Message           = 16
  };

  // Originated from server
  u_int16_t flags_;
  message_t message_;

  // Constant size does not take message into account
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  void execute(GameState*);
  size_t messageLength() const { return length_ - sizeOf(); }


  PDUEnableChat(const char* buf)
    :
    PDUHeader(buf),
    flags_  (0),
    message_(0)
  {
    const char* pbyte = &buf[PDU_HEADER_SIZE];

    u_int16_t* pint = (u_int16_t*)pbyte;
    flags_ = ntohs(*pint++);

    pbyte = (char*)pint;

    // Copy the message to a 0-terminated string
    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, pbyte, msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUEnableChat()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: ChatEnable PDU ---");
    PDUHeader::checkValid();
  }
};


END_NAMESPACE(Network)


#endif