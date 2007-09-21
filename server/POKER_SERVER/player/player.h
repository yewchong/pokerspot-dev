#ifndef __table_player_h__
#define __table_player_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPlayer
  Created  : 1/1/2001

  OVERVIEW : This class represents a player at a table.

 ****************************************************************************/


#include "def.h"
#include "includes.h"
#include "sync/sync.h"
#include "hand/hand.h"
#include "pingcounter.h"
#include "common/chips.h"
#include <list>
USE_STD;

class CChips;

/*
 *	PLAYER_STATE Defines : Different states that a player can be in.
 *
 *	First and foremost, it is important to know whether the player is sitting
 *	at the table, or not.  PLAYER_STATE_SITTING and PLAYER_STATE_OBSERVING
 *	provide this information.
 *
 *  NOTE: existing states must not be changed as the
 *  client relies on these values too.
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

class Cpdu;

class CPlayer
{
public:
  CPlayer(int slot);
  //CPlayer(int, const char*, const char*, unsigned int, int);
  ~CPlayer();

  void printPlayer();
  void stateChange(int);
  void setChips(const CChips&);
  void setChipsBehind(const CChips&);
  void setPlayer(int connfd,
                 const char* username,
                 const char* password,
                 const CChips& chips,
                 u_int32_t ipaddress);
  void setCity(const char* city);
  void setUnused();
  void setCurrentBet(const CChips&);
  
  int getConnfd() const;
  const char* getUsername() const;
  const char* getCity() const;
  CChips getChips() const;
  CChips getChipsBehind() const;  
  CChips getCurrentBet() const;
  int getState() const;
  int getSlot() const;
  
  bool matchConnfd(int);
  bool matchUsername(const char*);
  bool matchState(int);
  bool matchSlot(int);
  
  Chand* getHand();
  void killHand();
  bool hasHand() const;
  void setMuckCards(bool);
  bool getMuckCards() const;
  void setSitIn(bool);
  bool getSitIn() const;
  
  CChips getMissedBlind() const;
  void setMissedBlind(const CChips&);
  
  enum {
    Pdu_Wait_Timeout = -1
  };
  Cpdu* getFromQueue();
  bool  queuePdu(Cpdu*);
  int   waitPdu(Cpdu** ppPDU, DWORD timeOut, u_int16_t sequenceNumber);
  bool  checkSequenceNumber(Cpdu* pdu, u_int16_t sequenceNumber);
  
  bool isDisconnected() const;
  void setDisconnected(bool);  
  bool getSendError() const;
  void setSendError(bool);
  int getSitouts() const;
  void setSitouts(int);
  int getPings() const;
  void setPings(int);
  
  u_int32_t getIPAddress() const;
  
  int isZombie() const;
  void setIsZombie(bool);
  void doReanimate();
  void setReanimate(bool);
  bool reanimate() const;
  int  getTimeouts() const;
  void setTimeouts(int);
  
  bool isFreeSeat() const;
  void setIsFreeSeat(bool);
  
  bool isFirstHand() const;
  void setIsFirstHand(bool);
  
  CPingCounter* getPingCounter();
  
private:
  // Need 17 bytes to store 16 character
  // zero-terminated string
  char username_[16 + 1];
  char password_[16 + 1];
  char city_[16 + 1];

  int connfd_;
  int state_;
  int slot_;


  CChips chips_;
  CChips chipsBehind_;
  CChips currentBet_;
  CChips missedBlind_;

  Chand* hand_;
  bool muckCards_;
  bool sitIn_;
  bool disconnected_;
  
  int sitouts_;
  int pings_;
  
  bool sendError_;
  u_int32_t ipaddr_;
  
  bool isFreeSeat_;
  int  isZombie_;
  bool reanimate_;
  int  timeouts_;
  bool isFirstHand_;
  
  CPingCounter pingCounter_;
  
private:
  // Queue of incoming PDUs for this player
  typedef list<Cpdu*> PduQueue;
  PduQueue pduQueue_;
  // Access to list is controlled by a mutex
  Mutex pduMutex_;
  // Upon receiving a PDU, this event is set
  Event pduEvent_;
};

inline void CPlayer::setChips(const CChips& chips)
{
  chips_ = chips;
}

inline void CPlayer::setMuckCards(bool muckCards)
{
  muckCards_ = muckCards;
}

inline void CPlayer::setCurrentBet(const CChips& bet)
{
  currentBet_ = bet;
}

inline int CPlayer::getConnfd() const
{
  return connfd_;
};

inline int CPlayer::getState() const
{
  return state_;
};

inline CChips CPlayer::getChips() const
{
  return chips_;
};

inline CChips CPlayer::getChipsBehind() const
{
  return chipsBehind_;
};

inline void CPlayer::setChipsBehind(const CChips& chips)
{
  chipsBehind_ = chips;
};

inline const char* CPlayer::getCity() const
{
  return city_;
};

inline const char* CPlayer::getUsername() const
{
  return username_;
};

inline int CPlayer::getSlot() const
{
  return slot_;
}

inline CChips CPlayer::getCurrentBet() const
{
  return currentBet_;
}

inline bool CPlayer::getMuckCards() const
{
  return muckCards_;
}

inline bool CPlayer::isDisconnected() const
{
  return disconnected_;
}

inline bool CPlayer::hasHand() const
{
  return hand_ != NULL;
}

inline void CPlayer::setSitIn(bool b)
{
  sitIn_ = b;
}

inline bool CPlayer::getSitIn() const
{
  return sitIn_;
}

inline void CPlayer::setMissedBlind(const CChips& blind)
{
  missedBlind_ = blind;
}

inline int CPlayer::getSitouts() const
{
  return sitouts_;
}

inline void CPlayer::setSitouts(int s)
{
  sitouts_ = s;
}

inline int CPlayer::getPings() const
{
  return pings_;
}

inline void CPlayer::setPings(int p)
{
  pings_ = p;
}


inline bool CPlayer::getSendError() const
{
  return sendError_;
}

inline void CPlayer::setSendError(bool b)
{
  sendError_ = b;
}

inline u_int32_t CPlayer::getIPAddress() const
{
  return ipaddr_;
}

inline int CPlayer::isZombie() const
{
  return isZombie_;
}

inline bool CPlayer::isFreeSeat() const
{
  return isFreeSeat_;
}

inline void CPlayer::setIsFreeSeat(bool b)
{
  isFreeSeat_ = b;
}

inline CPingCounter* CPlayer::getPingCounter()
{
  return &pingCounter_;
}

inline void CPlayer::setReanimate(bool b)
{
  reanimate_ = b;
}

#endif
