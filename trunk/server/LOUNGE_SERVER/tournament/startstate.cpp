/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CStartState
  Created  : 07/07/2000

  OVERVIEW : This state implements the starting of a tournament.

 ****************************************************************************/

#include "tournament/startstate.h"
#include "tournament/runstateunfrozen.h"
#include "tournament/endstate.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/tournament.h"
#include "tournament/pduseatplayer.h"
#include "tournament/pdusynchronize.h"
#include "tournament/tournamentevent.h"
#include "tournament/seater.h"
#include "lounge/lounge.h"
#include "lounge/game.h"
#include "lounge/waitinglist.h"
#include "lounge/queueservice.h"
#include "pdu/pduplayerloginquery.h"
#include "pdu/pduqueuefull.h"
#include "sys.h"


#define SEATING_TIME (1 * 60)
#define SKIP_STEPS 3

CStartState::CStartState(CTournament* t)
    :
    CTournamentState(t),
    subState_       (Start),
    spawn_          (0),
    numTables_      (0),
    lastAction_     (0),
    skipSteps_      (SKIP_STEPS)
{
    t->logEvent(TE_Tournament, NULL, "Tournament starts");

    char buf[100];
    sprintf(buf, "tlog-%i.txt", t->getNumber());
    Sys_InitTournamentLog(buf);

    sprintf(buf, "Tournament %i starts", t->getNumber());
    Sys_LogTournament(buf);
}

void CStartState::tick(long now)
{
    CTournamentState::tick(now);

    CTournament* t = tournament_;

    CBaseTournamentQueue* tq =
      t->getTournamentQueue();
    if (!tq) return;

    // Spawn tables, seat players, send start
    if (subState_ == Start)
    {        
        if (tq->numPlayers() < 3)
        {
            // The tournament is cancelled!
            t->logEvent(TE_Tournament, NULL, "Tournament cancelled: too few players");
            t->addDescription("\r\nNOTE: The tournament was cancelled because there were not enough players.");
 
            setNextState(new CEndState(t));
            delete this;

            return;
        }

        subState_ = Spawn;

        int numPlayers = tq->numPlayers();
        
        const int LateComers = 20;
        int maxPlayersInTournament = tournament_->getMaxPlayers();

        int extraPlayers = LateComers;
        if (numPlayers + extraPlayers > maxPlayersInTournament)
        {
          extraPlayers = maxPlayersInTournament - numPlayers;
          extraPlayers = max(0, extraPlayers);
        }

        numPlayers += extraPlayers;

        // Compute the number of tables to spawn
        // so everybody will have a seat
        int tableMax = tq->getTableMax();
        spawn_ = ((numPlayers - 1) / tableMax) + 1;
  
#ifdef _WIN32
        cout << "\nPlayers: " << numPlayers - extraPlayers
         << " extra: " << extraPlayers << " tables: " << spawn_ << endl;
#endif

        numPlayers -= extraPlayers;

        if (spawn_ < 1)
            spawn_ = 1;

        numTables_ = spawn_;
        tournament_->setNumPlayers(numPlayers);
    }

    if (subState_ == Spawn)
    {
        if (spawn_ > 0)
        {
            if (t)
            {
                spawn_--;

                // XXX get these from tournament script
                CLounge::Inst()->spawnTable(t->getGameType(),
                                            tq->getGameType(),
                                            tq->getTableMax(),
                                            t->getBuyin(),
                                            t->getBuyin(),
                                            t->isHiLo()); 

            }
        }
        else
        {
            subState_ = Seat;
        }
    }

    TableList& tables = tq->getTableList();

    if (subState_ == Seat && (tables.size() == numTables_))
    {
        PlayerList& players = tq->getPlayers();

        // Create a new seating order
        seater_.createSeating(tq, tables, players, t->getStartChips());

        subState_ = Seating_Step;
    }

    if (subState_ == Seating_Step)
    {
        // Spread the seating over time so we wont
        // have 100 players logging in at once        
        if (skipSteps_-- > 0) return;
        skipSteps_ = SKIP_STEPS; 

        // Seat N players, move to next state if done
        bool done = seater_.stepSeating(tq, seated_);
        if (done)
        {
            // have to reschedule queue  since it is now empty
            CQueueService::Schedule(tq);

            subState_ = Seated0;
            return;
        }
    }

    if (subState_ == Seated0)
    {
        subState_ = Seated1;
    }

    if (subState_ == Seated1)
    {
        printf("Done seating - fill seats\n");
        // We're done seating players - tell tables to
        // fill the rest of seats with fake tournament players
        CpduTournamentParams pdu1;
        pdu1.sendTournamentParams(tq,
                                  TF_FillSeats,
                                  t->getStartChips().getRep(), 
                                  0);
        subState_ = Seated2;
        lastAction_ = now;
    }

    if (subState_ == Seated2)
    {   // Wait before running the tournament
        printf("Done seating - waiting to run\n");
        //if (now - lastAction_ > SEATING_TIME)
        //    subState_ = Done;
        subState_ = Done;
        waitReseat(seated_);
        return;
    }

    if (subState_ == Done)
    {
        CpduTournamentParams pdu1;
        pdu1.sendTournamentParams(tq, TF_Start, 0, 0);

        printf("Done - start running\n");
        Sys_Sleep(2000);

        CpduSynchronize pdu2;
        pdu2.sendSync(tq, SF_Play);

        setNextState(new CRunStateUnfrozen(tournament_));
        delete this;
    }
}


// Players cannot join while players are being seated.
bool CStartState::canAddPlayer(const CPlayer& p)
{
    CpduQueueFull pdu;
    pdu.sendQueueFull(tournament_->getTournamentQueue()->getQueueNumber(),
                      const_cast<CPlayer*>(&p),
                      QF_TournamentSeating);
    return false;
}