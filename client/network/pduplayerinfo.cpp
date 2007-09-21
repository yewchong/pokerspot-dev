/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPlayerInfo
  Created  : 03/01/2000

  OVERVIEW : Player Info PDU. This PDU is sent by the
             server at login to inform the client about
             players sitting at the table.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduplayerinfo.h"
#include "network/pduplayer.h"
#include "network/server.h"
#include "base/player.h"
#include "base/statewait.h"
#include "base/gamelogic.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/global.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_NAMESPACE0()

  LPCTSTR g_szPlayerJoined  = _T("%s sits in seat %d");

END_NAMESPACE0()


BEGIN_NAMESPACE(Network)
USING_NS_T(Base, GameLogic);
USING_NS_T(Base, StateWait);


PDUPlayerInfo::PDUPlayerInfo(const char* buf)
  :
  PDUHeader   (buf),
  num_players_(0),
  pPlayers_   (0)
{  
  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
  num_players_ = ntohs(*pint++);

  // Rest of the data is PlayerInfos - extract
  // from the buffer
  size_t extraBytes = length_ - sizeOf();
  u_int16_t num_players = extraBytes / sizeof(PlayerInfo);
  PDU_ASSERT(num_players == num_players_);
  num_players_ = num_players;

  if (num_players_ > 0)
    pPlayers_ = new PlayerInfo[num_players_];

  if (pPlayers_)
  { 
    memset(pPlayers_, 0, num_players_ * sizeof(PlayerInfo));
    PlayerInfo* pP = pPlayers_;

    for (int i = 0; i < num_players_; i++, pP++)
    {
      pP->slot_ = ntohs(*pint++);
      
      CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
      pP->chips_ = *pchips++; // XXX NOTE: in network byte order

      pint = (u_int16_t*)pchips;
      pP->state_ = ntohs(*pint++);
      u_byte_t* pbyte = (u_byte_t*)pint;
      memcpy(pP->username_, pbyte, PDU_STRINGSIZE);
      pbyte += PDU_STRINGSIZE;
      memcpy(pP->city_, pbyte, PDU_STRINGSIZE);
      pbyte += PDU_STRINGSIZE;
      pint = (u_int16_t*)pbyte;
    }
  }      
}


PDUPlayerInfo::~PDUPlayerInfo()
{
  delete [] pPlayers_;
  pPlayers_ = 0;
}

/*
 *  Copied from table server source!!!
 *
 *	PLAYER_STATE Defines : Different states that a player can be in.
 *
 *	First and foremost, it is important to know whether the player is sitting
 *	at the table, or not.  PLAYER_STATE_SITTING and PLAYER_STATE_OBSERVING
 *	provide this information.
 *
 */

#define PLAYER_STATE_SITTING		0x00FF	// All-1s mask for players at the table

#define PLAYER_STATE_LOGGEDIN       0x0001
#define PLAYER_STATE_BOUGHTIN       0x0002  // A player who has bought-in
#define PLAYER_STATE_WAITING        0x0004  // A player who has chips, and is ready to play
#define PLAYER_STATE_PLAYING		0x0008	// A player active in a current hand
#define PLAYER_STATE_FOLDED			0x0010	// A player who has folded the current hand
#define PLAYER_STATE_ALLIN			0x0020	// A player that is all-in on the current hand
#define PLAYER_STATE_ALLIN_RAISE    0x0040	// A player who raises and goes all-in 
#define PLAYER_STATE_SITOUT			0x0080	// A player who is sitting out the current hand
#define PLAYER_STATE_DONE           0x0100  // A player who CANNOT play anymore
#define PLAYER_STATE_TEMP           0x1000  // A player in a TEMPORARY state

#define PLAYER_STATE_READY          (PLAYER_STATE_BOUGHTIN | PLAYER_STATE_FOLDED | PLAYER_STATE_PLAYING | PLAYER_STATE_SITOUT)

#define PLAYER_STATE_ACTIVE         (PLAYER_STATE_PLAYING | PLAYER_STATE_FOLDED | PLAYER_STATE_ALLIN | PLAYER_STATE_WAITING)

#define PLAYER_STATE_STANDING		0x0100	// All-1s mask for players standing around
#define PLAYER_STATE_OBSERVING		0x0100	// A player that is watching the games

#define PLAYER_STATE_UNUSED         0x0200  // Player structure is not in use
#define PLAYER_STATE_LOGGINGOUT     0x2000  // A player who is on his way out
#define PLAYER_STATE_NOTRESPONDING  0x4000  // A player who is not responding to blind query
#define PLAYER_STATE_ZOMBIE         0x8000  // A player who is disconnect

  

void PDUPlayerInfo::execute(GameState*)
{
  CString msg;
  CTableView* pTable = CTableView::Instance();
  ASSERT(pTable);
  CChatView* pChat = CChatView::Instance();
  int num_players_b4_pdu = pTable->numPlayers();

  if (pTable)
  {
    int num_players = num_players_b4_pdu;

    PlayerInfo* pP = pPlayers_;
    if (pP != NULL)
    {
      for (int i = 0; i < num_players_; i++)
      {
        CString namesz = MakeString(pP->username_);
        Player* p = pTable->getPlayer(pP->slot_);
        bool wasThere = false;
        if (p && p->getName() == namesz)
          wasThere = true;

        pTable->addPlayer(namesz, CChips_n(pP->chips_).ntohl(), pP->slot_);

        // Set initial player state
        if (pP->state_ & PLAYER_STATE_ACTIVE)
          pTable->setState(pP->slot_, Base::SitIn);
        else
          pTable->setState(pP->slot_, Base::SitOut);

        Player* pPlayer = pTable->getPlayer(pP->slot_);

        ASSERT(pPlayer);
        if (pPlayer)
        {
          if (pP->state_ & PLAYER_STATE_ZOMBIE)
            pPlayer->setState(Base::Zombie);

          CString citysz = MakeString(pP->city_);
          pPlayer->setHomeCity(citysz);
          pTable->InvalidateRect(&pPlayer->getArea());  
        }

        if (pChat && !wasThere)
        {
          CString msg;
          msg.Format(g_szPlayerJoined, (LPCSTR)namesz, pP->slot_ + 1);
          pChat->addDealerMessage(msg, CChatView::CF_GameNormal);
        }
        
        pP++;
      }
    }

    // make a full redraw if original player
    // count was 0
    if (num_players == 0)
      pTable->Invalidate();
  }

  if (num_players_b4_pdu == 0)
  {
    // Send initial sit in status
    BOOL b = GameLogic::SitOut();
    if (Global::TableServer())
    {
      if (b)
        Global::TableServer()->sendPlaySitOutPDU();
      else
        Global::TableServer()->sendPlaySitInPDU();
    }

    // Send initial muck hand status
    b = GameLogic::HideHand();
    if (Global::TableServer())
    {
      if (b)
        Global::TableServer()->sendPlayHideCardsPDU();
      else
        Global::TableServer()->sendPlayShowCardsPDU();
    }

    // Done, start waiting for new hand to start
    GameLogic::Instance().setNextState(new StateWait());
  }
}



END_NAMESPACE(Network)