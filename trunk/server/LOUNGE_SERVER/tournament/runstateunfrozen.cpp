/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CRunStateUnfrozen
  Created  : 07/10/2000

  OVERVIEW : This state runs a tournament.

 ****************************************************************************/

#include "tournament/runstateunfrozen.h"
#include "tournament/runstate.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/tournamentevent.h"
#include "tournament/pdudissolvetable.h"
#include "tournament/waitstate.h"
#include "tournament/waitpause.h"
#include "tournament/pdusynchronize.h"
#include "pdu/pduchatenable.h"
#include "sys.h"


CRunStateUnfrozen::CRunStateUnfrozen(CTournament* t)
	:
    CTournamentState(t),
    startTime_      (0)
{}


void CRunStateUnfrozen::tick(long now)
{
    CTournamentState::tick(now);

    printf("Run unfrozen\n");

    CBaseTournamentQueue* tq =
      tournament_->getTournamentQueue();

    if (startTime_ == 0)
    {
      // First tick, enable chat
      CpduChatEnable pdu;
      pdu.broadcastChatEnable(CE_ChatEnabled, 0);

      startTime_ = now;
    }

    if (now - startTime_ > tournament_->getFreezeTime())
    {
		    // recount number of players: handles failed buy-ins
		    // and late comers
		    tournament_->setNumPlayers(tq->countPlayers());

        char buf[100];
        sprintf(buf, "Tournament is frozen: %i players",
                tournament_->getNumPlayers());
        tournament_->logEvent(TE_Tournament, NULL, buf);
        printf("Tournament is frozen\n");

        // Tell the tables to freeze
        CpduTournamentParams pdu;
        pdu.sendTournamentParams(tq,
                                 TF_Freeze,
                                 0, 0);

        // Update the player count in case late
        // comers have joined
        pdu.sendTournamentParams(tq,
                                 TF_Number,
                                 tournament_->getNumber(),
                                 tournament_->getNumPlayers());

        // Send pause
        CpduSynchronize pdu2;
        pdu2.sendSync(tq, SF_Pause);

        // Wait until all tables have done the freeze
        setNextState(new CWaitState(tournament_,
                                    new CRunState(tournament_, startTime_),
                                    new CWaitPause(tournament_, 120))); // wait no more than 2 minutes


        // This state is done
        delete this;
    }
}


// Players can be added until tournament is frozen
bool CRunStateUnfrozen::canAddPlayer(const CPlayer&)
{
    return true;
}


bool CRunStateUnfrozen::isUnfrozen() const
{
    return true;
}


bool CRunStateUnfrozen::allowRelogin() const
{
    return true;
}