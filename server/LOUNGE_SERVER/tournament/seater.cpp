/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSeater
  Created  : 07/26/2000

  OVERVIEW : This class implements the (re)seating of players in tournaments.

 ****************************************************************************/

#ifdef _WIN32
#pragma warning (disable : 4786)
#endif
#include "tournament/seater.h"
#include "tournament/pduseatplayer.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/tournamentevent.h"
#include "lounge/lounge.h"
#include "common/chips.h"
#include "pdu/pdunotify.h"
#include "dbhandler/dbinterface.h"
#include "sys.h"
#include <algorithm>

#define PIECEWISE_SEATING_NOT_YET_IMPLEMENTED_ 1

CSeater::CSeater()
:
    first_(true),
    seatingCounter_(0)
{}


// Seat 'players' randomly to 'tables'.
// This creates the initial seating and does the
// tournament buy-in.
void CSeater::createSeating(CBaseTournamentQueue* tq,
                            const TableList&      tables,
                            const PlayerList&     players,
                            const CChips&         chips)
{
#if PIECEWISE_SEATING_NOT_YET_IMPLEMENTED_
    first_ = true;
#endif
    players_.clear();
    tables_.clear();

    // Prepare a container of tables that have seats available
    for (TableList::const_iterator tb = tables.begin(),
                                   te = tables.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table->getNumPlayers() < tq->getTableMax())
            tables_.push_back(TableSeater(table, table->getNumPlayers()));
    }

    // Prepare another container for players
    for (PlayerList::const_iterator pb = players.begin(),
                                    pe = players.end();
         pb != pe;
         ++pb)
    {
        CPlayer* player = (*pb).player_.get();

        // Do the tournament buy-in and prepare to seat the player
        if (CdbInterface::Inst()->buyin(player, tq->getLo()))
        {
            players_.push_back(PlayerSeater(player->getUsername(), chips));
        }
        else
        {
            char s[200];
            sprintf(s, "Unable to perform $%d buy-in - tournament participation cancelled.",
                    tq->getLo());
            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getSocket(),
                                  s, NF_NoCloseConnection);

            sprintf(s, "Player %s tournament buyin failed\n", player->getUsername());
            Sys_LogError(s);
        }
    }

    // Shuffle the player container
    random_shuffle(players_.begin(), players_.end());
}


// Reseat 'players' to 'tables'.
void CSeater::createSeating(CBaseTournamentQueue* tq,
                            const TableList&      tables,
                            CTable*               dissolveTable,
                            dissolve_playerinfo*  dissolveInfo,
                            int                   numInfos)
{
#if PIECEWISE_SEATING_NOT_YET_IMPLEMENTED_
    first_ = true;
#endif
    players_.clear();
    tables_.clear();

    // Prepare a container of tables that have seats available
    for (TableList::const_iterator tb = tables.begin(),
                                   te = tables.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table != dissolveTable &&
            table->getNumPlayersSeated() < tq->getTableMax())
        {
            tables_.push_back(TableSeater(table, table->getNumPlayersSeated()));
        }
    }

    // Prepare another container for players
    for (int i = 0; i < numInfos; i++)
    {
      CChips chips = CChips_n(dissolveInfo->chips_).ntohl();
        if (chips > 0)
            players_.push_back(PlayerSeater(dissolveInfo->username_,
                                            chips));
        dissolveInfo++;
    }

    // Shuffle the player container
    random_shuffle(players_.begin(), players_.end());
}


void CSeater::commitSeating(CBaseTournamentQueue* tq,
                            bool                  immediate,
                            list<string>&         seatedPlayers)
{
    TableSeaters::iterator tb = tables_.begin();

    int loopGuard = 200;
    int i = 0;
    while (i < players_.size())
    {
        while (tb != tables_.end() && i < players_.size())
        {
            TableSeater& ts = (*tb);
            if (ts.numPlayers_ < tq->getTableMax())
            {
                PlayerSeater& ps = players_[i++];

                CpduSeatPlayer pdu;
                // We'll want seat acks unless seating is immediate
                pdu.sendSeatPlayer(ps.player_.c_str(),
                                   ps.chips_,
                                   !immediate ? SF_SendAck : 0,
                                   ts.table_); 

                ts.numPlayers_++;
                
                // Return seated players to caller
                seatedPlayers.push_back(ps.player_.c_str());

                if (immediate)
                {   
                    // Update the table list right now - if the
                    // seating fails for some reason, the player
                    // can log in back at the table
                    CLounge::Inst()->playerSeated(ps.player_.c_str(),
                                                  ts.table_);

                    CPlayer* player = findPlayer(ps.player_.c_str());
                    if (player)
                    {
                        // Do seating right away - this sends
                        // the actual pdus for seating
                        tq->seatPlayer(player, ts.table_);
                    }
                }
                else
                {
                    // The seating is done when receiving the seat ack pdu
                }
            }
            ++tb;
        }
        tb = tables_.begin();
    }
}


// XXX FIXME:
// If a table is removed while we're doing the seating,
// a TableSeater will have a pointer to a deleted table.
//
bool CSeater::stepSeating(CBaseTournamentQueue* tq,
                          list<string>&         seatedPlayers)
{
#if PIECEWISE_SEATING_NOT_YET_IMPLEMENTED_
    if (first_)
    {   // First call in piece-wise seating
        first_ = false;
        tb_ = tables_.begin();
    }

    const int StepSize = 10;
    int loopGuard = 200;
    int i = 0;
    // Seat 10 players at a time
    while (i < StepSize && seatingCounter_ < players_.size())
    {
        while (tb_ != tables_.end() &&
               i < StepSize &&
               seatingCounter_ < players_.size())
        {
            i++;

            TableSeater& ts = (*tb_);
            if (ts.numPlayers_ < tq->getTableMax())
            {
                //
                // This table has room, seat first player in
                // seaters list to this table
                //

                PlayerSeater& ps = players_[seatingCounter_++];

                CpduSeatPlayer pdu;
                pdu.sendSeatPlayer(ps.player_.c_str(),
                                   ps.chips_,
                                   0, // immediate, no acks
                                   ts.table_); 

                ts.numPlayers_++;
                
                // Return seated players to caller
                seatedPlayers.push_back(ps.player_.c_str());

                // Update the table list right now - if the
                // seating fails for some reason, the player
                // can log in back at the table
                CLounge::Inst()->playerSeated(ps.player_.c_str(),
                                              ts.table_);

                CPlayer* player = findPlayer(ps.player_.c_str());
                if (player)
                {
                    // Do seating right away - this sends
                    // the actual pdus for seating
                    tq->seatPlayer(player, ts.table_);
                }
            }
            ++tb_;
        }
        if (tb_ == tables_.end())
          tb_ = tables_.begin();
    }
    return !(seatingCounter_ < players_.size());
#endif
    return false;
}


CPlayer* CSeater::findPlayer(const char* username)
{
    return CLounge::Inst()->findPlayer(username);
}