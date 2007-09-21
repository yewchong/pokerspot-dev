/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournamentState
  Created  : 07/07/2000

  OVERVIEW : Base class for tournament states.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "tournament/tournament.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/waitstate.h"
#include "tournament/waitseated.h"
#include "lounge/table.h"
#include "pdu/pduqueuefull.h"
#include <stdio.h>

CTournamentState::CTournamentState(CTournament* t)
    :
    tournament_(t),
    ticks_     (0)
{}


CTournamentState::~CTournamentState()
{}

void CTournamentState::tick(long now)
{
    if (ticks_-- == 0)
    {
        dumpTables();
        ticks_ = 10;
    }
}

void CTournamentState::setNextState(CTournamentState* state)
{
    tournament_->setNextState(state);
}

// By default new players cannot be added to tournament queue.
bool CTournamentState::canAddPlayer(const CPlayer& p)
{
    CpduQueueFull pdu;
    pdu.sendQueueFull(tournament_->getTournamentQueue()->getQueueNumber(),
                      const_cast<CPlayer*>(&p),
                      QF_TournamentClosed);

    return false;
}

void CTournamentState::playerReseat(CTable*, const char*)
{
    // By default do nothing
}


void CTournamentState::dumpTables()
{
    printf("****\n");

    CBaseTournamentQueue* tq = tournament_->getTournamentQueue();

    TableList& tables = tq->getTableList();

    for (TableList::const_iterator tb = tables.begin(),
                                   te = tables.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        printf("%s #p %i #ps %i\n",
               table->getTitle().c_str(),
               table->getNumPlayers(),
               table->getNumPlayersSeated());

        printf("   Players: ");
        list<string>& plrs = table->getPlayers();
        for (list<string>::iterator pb = plrs.begin(),
                                    pe = plrs.end();
             pb != pe;
             ++pb)
        {
            printf("%s ", (*pb).c_str());
        }
        printf("\n");                            
    }
    printf("****\n");
}


void CTournamentState::waitReseat(const list<string>& players)
{
    // Waiting is done in separate state, after which
    // this state is activate again.
    setNextState(new CWaitState(tournament_,
                                this,
                                new CWaitSeated(players, 60)));
}


bool CTournamentState::isUnfrozen() const
{
  return false;
}

bool CTournamentState::allowRelogin() const
{
  return false;
}

bool CTournamentState::tableDissolveInProgress() const
{
  return false;
}
