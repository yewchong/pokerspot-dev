#ifndef __network_pdushowdown_h__
#define __network_pdushowdown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUShowdown
  Created  : 14.11.1999

  OVERVIEW : Showdown PDU.

 ****************************************************************************/

#include "network/network.h"
#include "network/pdudealcardsflex.h"

BEGIN_NAMESPACE(Network)


struct PDUShowdown : public PDUHeader
{
  u_int16_t player_;
  u_int16_t num_cards_;
  u_int16_t msglen_;
  message_t message_;

  CardEntry* pCards_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t); }
  size_t messageLength() const { return msglen_; }

  PDUShowdown(const char* buf);

  ~PDUShowdown()
  {
    delete pCards_;
    pCards_ = NULL;
    delete [] message_;
    message_ = NULL;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Showdown PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   numcards: %d message length: %d", num_cards_, msglen_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


#ifdef OLD_PDUSHOWDOWN_
struct PDUShowdown : public PDUHeader
{
  u_int16_t player_,
            card1_,
            suit1_,
            card2_,
            suit2_;
  u_int16_t msecs_;
  message_t message_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 6 * sizeof(u_int16_t); }
  size_t messageLength() const { return length_ - sizeOf(); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(player_);
    *pint++ = htons(card1_);
    *pint++ = htons(suit1_);
    *pint++ = htons(card2_);
    *pint++ = htons(suit2_);
    *pint++ = htons(msecs_);

    // Copy the message to 'buf'
    u_byte_t* pbyte = (u_byte_t*)pint;
    size_t msglen   = lstrlen(message_);
    size_t len      = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte           += msglen;
    *pbyte++        = '\0';

    return pbyte;
  }
/*
  u_byte_t* write(NetworkBuffer& buf) const
  {
    PDUHeader::write(buf);
    buf << player_;
    buf << card1_;
    buf << suit1_;
    buf << card2_;
    buf << suit2_;
    buf << msecs_;
    return buf.getpos();
  }
*/
  PDUShowdown(const char* username,
              const char* password,
              u_int16_t   player,
              u_int16_t   card1,
              u_int16_t   suit1,
              u_int16_t   card2,
              u_int16_t   suit2,
              u_int16_t   msecs,
              const char* msg, int msglen)
    :
    PDUHeader(PDU_Play_Showdown,
              sizeOf() + msglen + 1,
              username, password),
    player_  (player),
    card1_   (card1),
    suit1_   (suit1),
    card2_   (card2),
    suit2_   (suit2),
    msecs_   (msecs),
    message_ (0)
  {
    message_ = new char[msglen + 1];    
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDUShowdown(const char* buf)
    :
    PDUHeader(buf),
    player_  (0),
    card1_   (0),
    suit1_   (0),
    card2_   (0),
    suit2_   (0),
    msecs_   (0),
    message_ (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    player_ = ntohs(*pint++);
    card1_ = ntohs(*pint++);
    suit1_ = ntohs(*pint++);
    card2_ = ntohs(*pint++);
    suit2_ = ntohs(*pint++);
    msecs_ = ntohs(*pint++);

    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, (char*)pint, msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUShowdown()
  {
    delete [] message_;
    message_ = NULL;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Showdown PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   card2: %d suit2: %d msecs: %d", card2_, suit2_, msecs_);
    PDU_DEBUG((LPCSTR)s);
    s.Format("   player: %d card1: %d suit1: %d", player_, card1_, suit1_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};
#endif

END_NAMESPACE(Network)


#endif