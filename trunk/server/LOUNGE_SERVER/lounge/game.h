#ifndef __lounge_game_h__
#define __lounge_game_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CGame and CGameIterator
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             game.

             Each game we support (e.g., "Texas Hold'em", "Texas Hold'em Low",
             etc has a CGame instance in the CLounge's list.

             Each CGame has one CWaitingList a set of CTables.

 ****************************************************************************/

#include <string>
#include <list>
#include "compat.h"
#include "stl_compat.h"
#include "ptr.h"
#include "lounge/waitinglist.h"
USING_NS_T(std, string);
USING_NS_T(std, list);

class CGame;

typedef list<CWaitingList*> WaitingLists;
typedef list<CGame> Games;
typedef list<CGame>::iterator GameIT;
typedef list<CGame>::const_iterator GameCIT;

struct GameIterator
{
    GameIterator(const GameIT& begin,
                 const GameIT& end)
        : curr_(begin), end_(end) {}

    GameIterator(const GameIterator& rhs)
        : curr_(rhs.curr_), end_(rhs.end_) {}

    bool more() const
    {
        return curr_ != end_;
    }

    void next();
    CGame& current() const;

    GameIT curr_, end_;
};

struct GameIteratorC
{
    GameIteratorC(const GameCIT& begin,
                  const GameCIT& end)
        : curr_(begin), end_(end) {}

    GameIteratorC(const GameIteratorC& rhs)
        : curr_(rhs.curr_), end_(rhs.end_) {}

    bool more() const
    {
        return curr_ != end_;
    }

    void next();
    const CGame& current() const;

    GameCIT curr_, end_;
};

enum GameType
{
  GT_Holdem         = 0,
  GT_SevenStud      = 1,
  GT_SevenStud_HiLo = 2,
  GT_Omaha          = 3,
  GT_Omaha_HiLo     = 4
};

class CGame
{
public:
    CGame()
        : gameSubType_(0) {}
    CGame(const CGame& rhs)
        : gameSubType_(rhs.gameSubType_), gameName_(rhs.gameName_)
    {}
    CGame& operator = (const CGame& rhs)
    {
        if (this != &rhs)
        {
            gameSubType_ = rhs.gameSubType_;
            gameName_ = rhs.gameName_;
            waitingLists_ = rhs.waitingLists_;
        }
        return *this;
    }

    static CGame* AddGame(u_int16_t gameSubType,
                          const char* gameName);
    static CGame* GetGame(u_int16_t gameSubType);
    static CGame* GetTournament();
    static int    GetNumGames();

    // iterate over games
    static GameIterator GetGames();
    static GameIteratorC GetGamesC();

    ~CGame();

    // Add waiting list to a game
    CWaitingList* addWaitingList(int number,
                                 u_int16_t queueMax,
                                 u_int16_t tableMax,
                                 u_int16_t tableLow,
                                 u_int16_t tableHigh,
                                 int spawnThreshold);    
    WaitingLists& getWaitingLists();
    CWaitingList* getWaitingList(int number);
    CWaitingList* getWaitingList(u_int16_t lo,
                                 u_int16_t hi);

    GameType    getGameType() const;
    u_int16_t   getGameSubType() const;
    const char* getGameName() const;
    bool        isHiLoSplit() const;

    bool        isTournament() const;
    bool        isSatellite() const;

private:
    // This list maintains all games
    static list<CGame> games_;

    CGame(u_int16_t gameSubType, const char* gameName);
    CGame(u_int16_t gameSubType);

    u_int16_t gameSubType_;
    string    gameName_;

    // The waiting lists for this game
    WaitingLists waitingLists_;

    friend bool operator == (const CGame& lhs, const CGame& rhs);
};

inline u_int16_t CGame::getGameSubType() const
{
    return gameSubType_;
}

inline const char* CGame::getGameName() const
{
    return gameName_.c_str();
}

inline bool operator == (const CGame& lhs, const CGame& rhs)
{
    return lhs.gameSubType_ == rhs.gameSubType_;
}

inline WaitingLists& CGame::getWaitingLists()
{
    return waitingLists_;
}

inline void GameIterator::next()
{
    ++curr_;
}

inline CGame& GameIterator::current() const
{
    return (*curr_);
}

inline void GameIteratorC::next()
{
    ++curr_;
}

inline const CGame& GameIteratorC::current() const
{
    return (*curr_);
}

struct same_subtype
{
    same_subtype(u_int16_t gameSubType)
        : gameSubType_(gameSubType)
    {}
    bool operator() (const CGame& game)
    {
        return game.getGameSubType() == gameSubType_;
    }
    u_int16_t gameSubType_;
};


#endif
