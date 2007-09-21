#ifndef __network_pdublindcard_h__
#define __network_pdublindcard_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUBlindCard
  Created  : 12.12.1999

  OVERVIEW : Blind Card PDU.

 ****************************************************************************/

#include "network/network.h"

BEGIN_NAMESPACE(Network)


// Uses player slot index
struct PDUBlindCard : public PDUHeader
{
  u_int16_t player_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(player_);
    return (u_byte_t*)pint;
  }

  PDUBlindCard(const char* username,
               const char* password,
               u_int16_t   player)
    :
    PDUHeader(PDU_Play_BlindCard,
              sizeOf(),
              username, password),
    player_  (player)
  {
  }

  PDUBlindCard(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    player_ = ntohs(*pint++);
  }


  void checkValid()
  {
    PDU_DEBUG("--- Recv: Blind Card PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(player_ < 10);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   player: %d", player_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }    
};

// Uses player name
struct PDUBlindCard2 : public PDUHeader
{
  string_t player_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 PDU_STRINGSIZE; }

  u_byte_t* write(char* buf) const
  {    
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buf);
    memcpy(pbyte, player_, PDU_STRINGSIZE);
    return pbyte;
  }

  PDUBlindCard2(const char* username,
                const char* password,
                const char* player)
    :
    PDUHeader(PDU_Play_BlindCard,
              sizeOf(),
              username, password)
  {
    if (player && *player != '\0')
      strncpy(player_, player, PDU_STRINGSIZE);
  }

  PDUBlindCard2(const char* buf)
    :
    PDUHeader(buf)
  {
    u_byte_t* pbyte = (u_byte_t*)&buf[PDU_HEADER_SIZE];
    memcpy(player_, pbyte, PDU_STRINGSIZE);
  }


  void checkValid()
  {
    PDU_DEBUG("--- Recv: Blind Card PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   player: %s", player_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }    
};


END_NAMESPACE(Network)

#endif