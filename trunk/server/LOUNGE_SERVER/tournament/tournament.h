#ifndef __lounge_tournament_h__
#define __lounge_tournament_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Tournament
  Created  : 07/05/2000

  OVERVIEW : Tournament support.

 ****************************************************************************/

#include "psi_win32.h"
#include "compat.h"
#include "lounge/game.h"
#include "common/chips.h"
#include "tournament/event.h"
#include <string>
#include <list>

class CTournamentState;
class CPlayer;
class CTable;
class CBaseTournamentQueue;


class CTournament
{
public:
    CTournament(CBaseTournamentQueue*,
                int number,
                int maxPlayers,
                const CChips& buyin,
                const CChips& startChips,
                const CChips& startLow,
                u_int32_t startTime,
                const string& descr,
                const string& script);
    ~CTournament();

    void tick(long now);
    void setNextState(CTournamentState*);    
    string getTitle() const;

    CChips    getBuyin() const;
    CChips    getStartChips() const;
    CChips    getStartLo() const;
    int       getNumPlayers() const;
    int       getMaxPlayers() const;
    int       getNumPlayersPlaying() const;
    u_int32_t getStartTime() const;
    string    getDescription() const;
    int       getNumber() const;
    int       getFreezeTime() const;

    void setBuyin(const CChips&);
    void setStartChips(const CChips&);
    void setStartLo(const CChips&);
    void setNumPlayers(int);
    void setStartTime(u_int32_t);
    void setDescription(const string&);
    void addDescription(const char*);
    void setNumber(int);
    void setFreezeTime(int);

    void setGameType(GameType);
    void setIsHiLo(bool);
    GameType getGameType() const;
    bool isHiLo() const;

    bool isRunning() const { return isRunning_; }
    void setIsRunning(bool b) { isRunning_ = b; }

    bool hasEnded() const { return state_ == NULL; }

    bool isUnfrozen() const;
    bool allowRelogin() const;

    bool isPaused() const;
    void setIsPaused(bool);

    bool canAddPlayer(const CPlayer&);

    void playerReseat(CTable*     newTable,
                      const char* username);                      
    void waitReseat(const list<string>& players);

    bool logEvent(int eventId,
                  const char* player,
                  const char* extra);

    CBaseTournamentQueue* getTournamentQueue() const;

    bool tableDissolveInProgress() const;

private:
    int       number_;
    CChips    buyin_, startChips_, startLo_;
    u_int32_t startTime_;
    int       numPlayers_;
    int       maxPlayers_;
    int       freezeTime_;
    string    description_;

    GameType  gameType_;
    bool      isHilo_;

    CTournamentState* state_;

    // This tournament belongs to this queue
    CBaseTournamentQueue* tournamentQueue_;

    bool isRunning_;
    bool isPaused_;

    bool flipFlop_;

public:
    void dumpEvents();
    void clearEvents();
    void addEvent(CTimedEvent* e) { timedEvents_.push_back(e); }
    void addEvent(CPlayerEvent* e) { playerEvents_.push_back(e); }
    void addEvent(CTableEvent* e) { tableEvents_.push_back(e); }

    list<CTimedEvent*>& timedEvents() { return timedEvents_; }
    list<CPlayerEvent*>& playerEvents() { return playerEvents_; }
    list<CTableEvent*>& tableEvents() { return tableEvents_; }

private:
    // These events are generated from the tournament script;
    // they define the tournament format.
    list<CTimedEvent*> timedEvents_;
    list<CPlayerEvent*> playerEvents_;
    list<CTableEvent*> tableEvents_;
};


inline CChips CTournament::getBuyin() const
{
    return buyin_;
}

inline CChips CTournament::getStartChips() const
{
    return startChips_;
}

inline CChips CTournament::getStartLo() const
{
    return startLo_;
}

inline string CTournament::getDescription() const
{
    return description_;
}

inline void CTournament::setBuyin(const CChips& b)
{
    buyin_ = b;
} 

inline void CTournament::setStartChips(const CChips& c)
{
    startChips_ = c;
}

inline void CTournament::setStartLo(const CChips& l)
{
    startLo_ = l;
}

inline void CTournament::setStartTime(u_int32_t t)
{
    startTime_ = t;
}

inline void CTournament::setDescription(const string& d)
{
    description_ = d;
}

inline void CTournament::addDescription(const char* p)
{
    description_ += p;
}

inline void CTournament::setNumber(int n)
{
    number_ = n;
}

inline int CTournament::getNumber() const
{
    return number_;
}

inline int CTournament::getFreezeTime() const
{
    return freezeTime_;
}

inline void CTournament::setFreezeTime(int f)
{
    freezeTime_ = f;
}

inline bool CTournament::isPaused() const
{
    return isPaused_;
}

inline void CTournament::setIsPaused(bool isPaused)
{
    isPaused_ = isPaused;
}

inline void CTournament::setGameType(GameType gt)
{
    gameType_ = gt;
}

inline void CTournament::setIsHiLo(bool b)
{
    isHilo_ = b;
}

inline GameType CTournament::getGameType() const
{
    return gameType_;
}

inline bool CTournament::isHiLo() const
{
    return isHilo_;
}

inline CBaseTournamentQueue* CTournament::getTournamentQueue() const
{
  return tournamentQueue_;
}

inline int CTournament::getMaxPlayers() const
{
  return maxPlayers_;
}

#endif