#ifndef __network_pduqueueupdate_h__
#define __network_pduqueueupdate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUQueueUpdate
  Created  : 03/13/2000

  OVERVIEW : Lounge server sends this PDU to the lounge client
             to notify a change in the queue state. The change
             can be either player add/remove or table add/remove
             event.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

// This version of queue update contains
// the names of the players
struct PDUQueueUpdate2 : public PDUHeader
{
  // Originated from server
  u_int16_t numActions_;
  // numActions_ times; only 1 for now...
  struct QueueDelta
  {
    u_int16_t queue_;
    u_int16_t action_; 
    char      name_[PDU_STRINGSIZE];
  };
  QueueDelta* pDeltas_;

  enum {
    Action_Player_Add        = 1,
    Action_Player_Remove     = 2,
    Action_Player_Serviced   = 3,
    Action_Table_Add         = 4,
    Action_Table_Remove      = 5,
    Action_Tournament_Add    = 6,
    Action_Tournament_Remove = 7,
  };

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUQueueUpdate2(const char* buf)
    :
    PDUHeader(buf),
    numActions_(0),
    pDeltas_   (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    numActions_ = ntohs(*pint++);

    // Rest of the data is PlayerInfos - extract
    // from the buffer
    size_t extraBytes = length_ - sizeOf();
    u_int16_t numActions = extraBytes / sizeof(QueueDelta);
    PDU_ASSERT(numActions == numActions_);
    numActions_ = numActions;

    if (numActions_ > 0)
      pDeltas_ = new QueueDelta[numActions_];

    if (pDeltas_)
    { 
      memset(pDeltas_, 0, numActions_ * sizeof(QueueDelta));
      QueueDelta* pD = pDeltas_;

      for (int i = 0; i < numActions_; i++, pD++)
      {
        pD->queue_ = ntohs(*pint++);
        pD->action_ = ntohs(*pint++);
        char* pbyte = (char*)pint;
        memcpy(pD->name_, pbyte, PDU_STRINGSIZE);
        pbyte += PDU_STRINGSIZE;
        pint = (u_int16_t*)pbyte;
        pD->name_[PDU_STRINGSIZE - 1] = '\0';
      } 
    }
  }

  ~PDUQueueUpdate2()
  {
    delete pDeltas_;
    pDeltas_ = NULL;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Queue Update PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   updates: %d", numActions_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDUQueueUpdate : public PDUHeader
{
  // Originated from server
  u_int16_t queue_;
  u_int16_t size_; 

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t); }

  PDUQueueUpdate(const char* buf)
    :
    PDUHeader(buf),
    queue_   (0),
    size_    (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    queue_ = ntohs(*pint++);
    size_  = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Queue Update PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   queue: %d size: %d", queue_, size_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif