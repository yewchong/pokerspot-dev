#ifndef __network_pduqueuefull_h__
#define __network_pduqueuefull_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUQueueFull
  Created  : 05/15/2000

  OVERVIEW : Lounge server sends this PDU to the lounge client
             to notify that an attempt to join a queue failed
             because the queue is full.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUQueueFull : public PDUHeader
{
  // Originated from server
  u_int16_t queueNumber_;
  u_int16_t reason_;

  enum Reason
  {
      QueueFull        = 0,
      NotEnoughChips   = 1,
      TournamentClosed = 2,
      AlreadyPlaying   = 3,
      TournamentSeating = 4,
      NotValidTournament = 5
  };


  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t); }

  PDUQueueFull(const char* buf)
    :
    PDUHeader   (buf),
    queueNumber_(0),
    reason_     (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    queueNumber_ = ntohs(*pint++);
    reason_ = ntohs(*pint++);
  }

  ~PDUQueueFull()
  {}

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Queue Full PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   queue: %d", queueNumber_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif