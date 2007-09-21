#ifndef __lounge_pduloungetransactionreply_h__
#define __lounge_pduloungetransactionreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeTransactionReply
  Created  : 05/16/2000

  OVERVIEW : Lounge Transaction Reply PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDULoungeTransactionReply : public PDUHeader
{
  // Originated from server
  message_t message_;

  void execute(GameState*);

  size_t messageLength() const { return length_ - sizeOf(); }

  PDULoungeTransactionReply(const char* buf)
    :
    PDUHeader(buf),
    message_ (0)
  {
    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, &buf[PDU_HEADER_SIZE], msglen);
        message_[msglen] = '\0';
      }
    }
  }

  PDULoungeTransactionReply()
    :
    PDUHeader(PDU_Lounge_TransactionReply,
              sizeOf(), NULL, NULL),
    message_(0)
  {}

  ~PDULoungeTransactionReply()
  {
    delete [] message_;
    message_ = NULL;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Transaction Reply PDU ---");
    PDUHeader::checkValid();
  }

};


END_NAMESPACE(Network)

#endif