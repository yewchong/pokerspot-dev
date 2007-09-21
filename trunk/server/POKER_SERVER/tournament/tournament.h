#ifndef __table_tournament_h__
#define __table_tournament_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournament
  Created  : 07/11/2000

  OVERVIEW : Tournament manager class for table server.

 ****************************************************************************/

#include "psi_win32.h"
#include "sync/sync.h"
#include "tournament/pdusynchronize.h"
#include "common/chips.h"
#include <vector>
USING_NS_T(std, vector);


class CPlayer;
class CTable;
class CChips;


class CTournament
{
public:
  static CTournament* Inst();
  
  bool isTournament() const;
  bool getDissolve() const;
  void isTournament(bool);
  void setDissolve(bool);
  
  void getLimits(CChips& lolimit, CChips& hilimit) const;
  u_int16_t getBettingStructure() const;
  u_int16_t getGameType() const;
  bool isHiLo() const;
  
  void setTournamentNumber(int);
  int  getTournamentNumber() const;
  void setNumPlayers(int);
  
  void setLimits(const CChips& lolimit, const CChips& hilimit);
  void setGameType(u_int16_t gameType);
  void setBettingStructure(u_int16_t betStructure);
  void setIsHiLo(bool);
  
  SyncFlag getSyncState() const;
  void setSyncState(SyncFlag);
  
  bool isFrozen() const;
  void setIsFrozen(bool);
  
  bool pause() const;
  void setPauseTime(u_int16_t);
  u_int16_t getPauseTime() const;
  long getPauseStartTime() const;
  
  void playerFinished(CPlayer*);
  void dissolveTable();
  void declareWinner(CTable*);
  
  bool logEvent(int eventId,
                const char* username,
                const char* extra);
  
  void makeEliminationOrder();
  int checkEliminated();

public:
  CTournament();

private:
  bool isTournament_;
  bool dissolve_;
  
  int number_, numPlayers_;
  CChips lolimit_, hilimit_;
  u_int16_t gameType_, betStructure_;
  bool      hilo_;
  
  u_int16_t pauseTime_;
  long      pauseStart_;
  
  SyncFlag  syncState_;
  bool isFrozen_;
  
  Mutex mutex_;
  
  static CTournament* pInst_;
  
  vector<CPlayer*> eliminationOrder_;
};

inline bool CTournament::isTournament() const
{
  return isTournament_;
}

inline bool CTournament::getDissolve() const
{
  return dissolve_;
}

inline void CTournament::isTournament(bool b)
{
  isTournament_ = b;
}

inline void CTournament::setDissolve(bool b)
{
  dissolve_ = b;
}

inline SyncFlag CTournament::getSyncState() const
{
  return syncState_;
}

inline void CTournament::setSyncState(SyncFlag s)
{
  if (dissolve_ && s == SF_Play)
  {
    // Dissolve overrides sync states - no more hands
  }
  else
  {
    syncState_ = s;
    if (syncState_ == SF_Play)
      pauseTime_ = 0;
  }
}

inline bool CTournament::isFrozen() const
{
  return isFrozen_;
}

inline void CTournament::setIsFrozen(bool b)
{
  isFrozen_ = b;
}

inline void CTournament::setNumPlayers(int n)
{
  numPlayers_ = n;
}

inline u_int16_t CTournament::getPauseTime() const
{
  return pauseTime_;
}

inline long CTournament::getPauseStartTime() const
{
  return pauseStart_;
}

inline bool CTournament::isHiLo() const
{
  return hilo_;
}

inline void CTournament::setIsHiLo(bool b)
{
  hilo_ = b;
}

inline int CTournament::getTournamentNumber() const
{
  return number_;
}

#endif
