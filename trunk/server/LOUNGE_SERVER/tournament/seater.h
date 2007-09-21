#ifndef __lounge_seater_h__
#define __lounge_seater_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSeater
  Created  : 07/26/2000

  OVERVIEW : This class implements the (re)seating of players in tournaments.

 ****************************************************************************/

#include "lounge/waitinglist.h"
#include "tournament/pdudissolveinfo.h"
#include "common/chips.h"
#include <list>
#include <string>

class CBaseTournamentQueue;

class CSeater
{
public:
    CSeater();

    // Seat 'players' randomly to 'tables'.
    void createSeating(CBaseTournamentQueue*,
                       const TableList&  tables,
                       const PlayerList& players,
                       const CChips&     chips);
    // Reseat 'players' to 'tables'.
    void createSeating(CBaseTournamentQueue*,
                       const TableList&     tables,
                       CTable*              dissolveTable,
                       dissolve_playerinfo* dissolveInfo,
                       int                  numInfos);

    // Commit seating
    void commitSeating(CBaseTournamentQueue*, bool immediate,
                       list<string>& seated);
    bool stepSeating(CBaseTournamentQueue*, list<string>& seated);

private:
    CPlayer* findPlayer(const char* username);

    struct PlayerSeater
    {
        string player_;
        CChips chips_;
        PlayerSeater(const string& p, const CChips& c)
            : player_(p), chips_(c) {}
    };
    typedef vector<PlayerSeater> PlayerSeaters;
    PlayerSeaters players_;

    struct TableSeater
    {
        CTable* table_;
        int     numPlayers_;
        TableSeater(CTable* t, int n)
            : table_(t), numPlayers_(n) {}
    };
    typedef list<TableSeater> TableSeaters;
    TableSeaters tables_;

// piecewise seating:
    bool first_;
    TableSeaters::iterator tb_;
    int seatingCounter_;
};


#endif