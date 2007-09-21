#ifndef __table_table_h__
#define __table_table_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CTable
  Created  : 1/1/2001

  OVERVIEW : This class maintains the state of a poker table.

 ****************************************************************************/


#include "includes.h"
#include "deck/deck.h"
#include "pot/pot.h"
#include "hand/hand.h"
#include "dbinterface/dbinterface.h"
#include "sync/sync.h"
#include "loginentry.h"

// Forward declares
class Cpdu;
class Cring;
class CPlayer;
class CpduLogin;
class CpduLogout;
class CpduBuyinRequest;
class CpduObserverLogin;
class CpduObserverLogout;
class CChips;


// Defaults
#define DEFAULT_HI      CChips(10, 0)
#define DEFAULT_LOW     CChips(5, 0)


enum Game_Type
{
  Game_Type_Holdem            = 0,
  Game_Type_SevenStud         = 1,
  Game_Type_SevenStud_HiLo    = 2,
  Game_Type_Omaha             = 3,
	Game_Type_Omaha_HiLo        = 4
};


class CTable
{
public:
  CTable();
  ~CTable();
  static CTable* Instance();

  void setGameType(unsigned int);
  void setQueueIndex(unsigned int);
  void setHi(const CChips&);
  void setLo(const CChips&);
  void setSeed(unsigned int);
  void setPort(unsigned int);
  void setConnfd(unsigned int);
  void setButtonSlot(unsigned int);
  bool advanceButton();
  void setBet(const CChips&);
  void setMaxPlayers(u_int16_t);

  void setDbase(CdbHandler*);
  CdbInterface* getDbase();

  Game_Type getGameType() const;
  unsigned int getSeed() const;
  CChips getHi() const;
  CChips getLo() const;
  unsigned int getPort() const;
  unsigned int getConnfd() const;
  unsigned int getButtonSlot() const;
  unsigned int getMaxPlayers() const;
  CPlayer* getPlayerFromSlot(int);
  CPlayer* getPlayerFromSocket(SOCKET);
  CPlayer* getUnusedPlayer(const char* username);
  Cring* getRing() const;
  Cring* getDealer();
  Cring* getRingNode(CPlayer*) const;
  Cpot* getMainPot();
  Cpot* getPot() const;
  Cpot* getLivePot();
  CChips getBet() const;

  u_int32_t getGameNumber() const;
  void setGameNumber();
  int countPlayers(int);
  int  numPlayersSittingIn() const;
  void tableLogin(SOCKET, CpduLogin*);
  void tableLogout(SOCKET, CpduLogout*);

  void newDeck();
  void shuffleDeck();
  void dealCards(CPlayer*, int);
  void dealUpCards(CPlayer*, int);
  void dealCards(Cring*, int);
  int  getDeckNumCards() const;
  void killPots();
  void killHands();
  bool openLog();
  bool setLog(const char*);
  bool flushLog();
  Chand* getBoard();
  void distributeBoard();
  
  u_int16_t checkShutdown();
  u_int16_t getShutdown();
  bool      notifyShutdown() const;
  void      setShutdown(u_int16_t, bool saveLog = true);
  
  CChips getAveragePotSize() const;
  u_int16_t getFlopPercentage() const;
  void updatePotAverage();
  void updateFlopPercentage();
  bool checkLogin(u_int32_t ipaddr, const char* username);
  bool gameHistoryUpdate(CPlayer*);
  bool putHandLogRequest(SOCKET, const char*, int);
  
  bool savePlayerChips(CPlayer*);
  void disconnectPlayer(SOCKET);
  
  bool canObserve(u_int32_t ipaddr, const char* username);
  void observerLogin(SOCKET, CpduObserverLogin*);
  void observerLogout(SOCKET, CpduObserverLogout*);
  CPlayer* getObserver(int index);
  
  enum {
    MaxPlayers = 10,
    MaxObservers = 20
  };
  int countPlayersRaw(bool countZombies = true) const;
  CPlayer* getPlayerRawSlot(int slot) const;
  CPlayer* getPlayerRawSocket(int slot) const;
  Mutex& getTableMutex();
  
  bool seatPlayer(const char* username, const CChips& chips, bool checkZombie);
  void fillSeats(const CChips& chips);
  void freezeTournament();
  void reseatPlayer(const char* username, u_int32_t addr, u_int16_t port);
  CPlayer* findPlayer(const char* username);
  CPlayer* findZombie(const char* username);
  bool checkDoubleLogin(const char* username, CPlayer*);
  void resetMissedBlinds();

#ifdef DEBUG
  void printTable();
#endif

private:
  Game_Type gameType_;
  unsigned int queueIndex_;
  unsigned int seed_;
  CChips hi_;
  CChips low_;
  unsigned int port_;
  unsigned int connfd_;
  unsigned int buttonslot_;

  unsigned int flopprcnt_;
  CChips potavg_;
  CChips totalPots_;
  unsigned long numPots_;
  unsigned int tableMax_;
  Cpot pot_;
  Cpot* activePot_;
  CChips bet_;
  CDeck deck_;
  Cring* ring_;
  CdbInterface* dbase_;
  u_int32_t gameNumber_;
  Chand* board_;

  CPlayer* players_[MaxPlayers];
  CPlayer* observers_[MaxObservers];
  // Access to players is controlled with a mutex
  Mutex tableMutex_;

private:
  static CTable* pInstance_;
  u_int16_t shutdown_;
  bool      notifyShutdown_;

public:
  // Implement login tracking
  void tick(long now);
  void addLoginEntry(const char* username,
                     u_int32_t ipaddress);

private:
  LoginEntries loginEntries_;
};

inline CChips CTable::getBet() const
{
  return bet_;
};

inline u_int32_t CTable::getGameNumber() const
{
  return gameNumber_;    
};

inline unsigned int CTable::getConnfd() const
{
  return connfd_;
};

inline unsigned int CTable::getButtonSlot() const
{
  return buttonslot_;
};

inline Game_Type CTable::getGameType() const
{
  return gameType_;
};

inline CChips CTable::getHi() const
{
  return hi_;
};

inline CChips CTable::getLo() const
{
  return low_;
};

inline unsigned int CTable::getPort() const
{
  return port_;
};

inline unsigned int CTable::getSeed() const
{
  return seed_;
};

inline Cpot* CTable::getMainPot()
{
  return &pot_;
};

inline Cpot* CTable::getPot() const
{
  return activePot_;
};

inline Cring* CTable::getRing() const
{
  return ring_;
}

inline CTable* CTable::Instance()
{
  return pInstance_;
}

inline CPlayer* CTable::getPlayerRawSlot(int slot) const
{
  if (slot >= 0 && slot < MaxPlayers)
    return players_[slot];
  else
    return NULL;
}

inline Mutex& CTable::getTableMutex()
{
  return tableMutex_;
}

inline void CTable::setMaxPlayers(u_int16_t tableMax)
{
  tableMax_ = tableMax;
}

inline unsigned int CTable::getMaxPlayers() const
{
  return tableMax_;
}

inline CPlayer* CTable::getObserver(int index)
{
  return observers_[index];
}

inline int CTable::getDeckNumCards() const
{
  return deck_.getNumCards();
}

inline void CTable::setQueueIndex(unsigned int qi)
{
  queueIndex_ = qi;
}

inline bool CTable::notifyShutdown() const
{
  return notifyShutdown_;
}

#endif
