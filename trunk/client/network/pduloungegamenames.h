#ifndef __network_pdugamenames_h__
#define __network_pdugamenames_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : pdugamenames.h
  Created  : 05/08/2000

  OVERVIEW : Lounge Game Names PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUGameNames : public PDUHeader
{
  // Originated from server

  u_int16_t numGames_;

  // numGames_ times <type, length, string> triplets

  struct GameName
  {
      u_int16_t gameType_;
      string    gameName_;
  };
  GameName* pNames_;

#ifndef PSPOT_TABLE_MODULE_
  void execute(GameState*);
#endif

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUGameNames(const char* buf)
    :
    PDUHeader (buf),
    numGames_ (0),
    pNames_   (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    numGames_ = ntohs(*pint++);

    if (numGames_ > 0)
    {
      const int MaxName = 1024;
      char* namebuf = new char[MaxName];
      pNames_ = new GameName[numGames_];
      GameName* pG = pNames_;

      if (namebuf && pNames_)
      {
        for (int i = 0; i < numGames_; i++, pG++)
        {
          pG->gameType_ = ntohs(*pint++);
          u_int16_t length = ntohs(*pint++);
          u_byte_t* pbyte = (u_byte_t*)pint;
          if (length < MaxName)
          {
            memcpy(namebuf, pbyte, length);
            namebuf[length] = 0;
            pbyte += length;
          }
          else
          {
            memcpy(namebuf, pbyte, MaxName-1);
            namebuf[MaxName-1] = 0;
            pbyte += (MaxName-1);
          }
          pG->gameName_ = (char*)namebuf;
          pint = (u_int16_t*)pbyte;
        }
      }

      delete [] namebuf;
    }
  }

  ~PDUGameNames()
  {
    delete [] pNames_;
    pNames_ = NULL;
  }
    
  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Game Names PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("  Games: %d", numGames_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};

END_NAMESPACE(Network)

#endif