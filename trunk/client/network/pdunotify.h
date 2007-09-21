#ifndef __network_pdunotify_h__
#define __network_pdunotify_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : PDUNotify
  Created  : 04/03/2000

  OVERVIEW : Notify PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUNotify : public PDUHeader
{ // Originated from server

  enum NotifyFlag
  {
      // Message will be displayed in status bar
      // instead of a dialog
      NF_StatusMessage = 1,

      // The client will not close the socket
      // upon receiving this PDU
      NF_NoCloseConnection = 2
  };

  u_int16_t flags_;
  message_t message_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }
  size_t messageLength() const { return length_ - sizeOf(); }

  void execute(GameState*);

  PDUNotify(const char* buf)
    :
    PDUHeader(buf),
    flags_   (0),
    message_ (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    flags_ = ntohs(*pint++);
    char* pchar = (char*)pint;

    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, pchar, msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUNotify()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Server Notify PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    if (message_ != 0)
    {
      CString s;
      s.Format("   flags: %d", flags_);
      PDU_DEBUG((LPCSTR)s);

      char buf[10];
      strncpy(buf, message_, 9);
      buf[9] = '\0';
      s.Format("   start of message: %s", message_);
      PDU_DEBUG((LPCSTR)s);
    }
#endif
  }
};


END_NAMESPACE(Network)


#endif