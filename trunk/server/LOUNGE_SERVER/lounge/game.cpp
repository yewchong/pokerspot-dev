/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CGame
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             game.

             Each game we support (e.g., "Texas Hold'em", "Texas Hold'em Low",
             etc has a CGame instance in the Lounge's list.

             Each CGame has one CWaitingList a set of CTables.

 ****************************************************************************/

#pragma warning( disable : 4786 )
#include "lounge/game.h"
#include "tournament/tournamentqueue.h"
#include "tournament/satellitequeue.h"

// The list of games
list<CGame> CGame::games_;

// Add game of type 'gameSubType, return it.
// If game 'gameSubType' already exists, no game
// is added.
CGame* CGame::AddGame(u_int16_t gameSubType,
                      const char* gameName)
{
    // If we already have a game of this type,
    // dont create again - return the same
    Games::iterator p = find_if(games_.begin(),
                                games_.end(),
                                same_subtype(gameSubType));
    if (p == games_.end())
    {
        CGame newGame(gameSubType, gameName);
        games_.push_back(newGame);
        // return ptr to the new game
        CGame& g = games_.back();
        return &g;
    }
    else
    {   // return ptr to the existing game
        CGame& g = *p;
        return &g;
    }

    return NULL;
}



int CGame::GetNumGames()
{
    return CGame::games_.size();
}

CGame* CGame::GetGame(u_int16_t gameSubType)
{
    Games::iterator pos = find_if(games_.begin(),
                                  games_.end(),
                                  same_subtype(gameSubType));
    if (pos != games_.end())
    {
        // return ptr to the game
        CGame& g = *pos;
        return &g;
    }

    return NULL;
}

GameIterator CGame::GetGames()
{
    return GameIterator(games_.begin(), games_.end());

}

GameIteratorC CGame::GetGamesC()
{
    return GameIteratorC(games_.begin(), games_.end());
}

CGame::CGame(u_int16_t gameSubType,
             const char* gameName)
    :
    gameSubType_(gameSubType),
    gameName_(gameName)
{}

CGame::CGame(u_int16_t gameSubType)
    :
    gameSubType_(gameSubType)
{}

CGame::~CGame()
{
    for (WaitingLists::iterator b = waitingLists_.begin(),
                                e = waitingLists_.end();
         b != e;
         ++b)
    {
        delete (*b);
    }
    waitingLists_.clear();
}


CWaitingList* CGame::addWaitingList(int number,
                                    u_int16_t queueMax,
                                    u_int16_t tableMax,
                                    u_int16_t tableLow,
                                    u_int16_t tableHigh,
                                    int       spawnThreshold)
{
    if (isTournament())
    {   
        // multitable tournament waiting list
        waitingLists_.push_back(new CTournamentQueue(
                                               this,
                                               number,
                                               getGameSubType(),
                                               queueMax,
                                               tableMax,
                                               tableLow,
                                               tableHigh,
                                               spawnThreshold));
    }
    else if (isSatellite())
    {
        // satellite tournament waiting list
        waitingLists_.push_back(new CSatelliteQueue(
                                      this,
                                      number,
                                      getGameSubType(),
                                      queueMax,
                                      tableMax,
                                      tableLow,
                                      tableHigh,
                                      spawnThreshold));
    }
    else
    {   // ring game waiting list
        waitingLists_.push_back(new CWaitingList(
                                            this,
                                            number,
                                            getGameSubType(),
                                            queueMax,
                                            tableMax,
                                            tableLow,
                                            tableHigh,
                                            spawnThreshold));
    }
    // return ptr to the newly created waiting list
    return waitingLists_.back();
}

// Return the first waiting list with
// specified limits.
CWaitingList* CGame::getWaitingList(u_int16_t lo,
                                    u_int16_t hi)
{
    WaitingLists::iterator pos = find_if(waitingLists_.begin(),
                                         waitingLists_.end(),
                                         same_limits(lo, hi));
    if (pos != waitingLists_.end())
    {
        CWaitingList* wl = *pos;
        return wl;
    }

    return NULL;
}


// Return the first waiting list with
// specified number.
CWaitingList* CGame::getWaitingList(int number)
{
    WaitingLists::iterator pos = find_if(waitingLists_.begin(),
                                         waitingLists_.end(),
                                         same_number(number));
    if (pos != waitingLists_.end())
    {
        CWaitingList* wl = *pos;
        return wl;
    }

    return NULL;
}


GameType CGame::getGameType() const
{
    if (gameName_.find("Hold'em") != string::npos)
        return GT_Holdem;
    else if (gameName_.find("Stud") != string::npos)
        return GT_SevenStud;
    else if (gameName_.find("Omaha") != string::npos)
        return GT_Omaha;
    else
        return GT_Holdem;
}


CGame* CGame::GetTournament()
{
    for (Games::iterator b = games_.begin(),
                         e = games_.end();
         b != e;
         ++b)
    {
         CGame& g = (*b);
         if (g.isTournament())
             return &g;
    }

    return NULL;
}

bool CGame::isHiLoSplit() const
{
    return gameName_.find("Hi/Lo") != string::npos;
}

bool CGame::isTournament() const
{
    return gameName_.find("Tournament") != string::npos;
}

bool CGame::isSatellite() const
{
    return gameName_.find("Satellite") != string::npos;
}

