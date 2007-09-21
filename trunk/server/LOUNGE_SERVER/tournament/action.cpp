/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CAction
  Created  : 08/04/2000

  OVERVIEW : These classes implement different actions for tournaments.

 ****************************************************************************/

#include "tournament/action.h"
#include "tournament/pdutournamentparams.h"
//#include "tournament/tournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/tournamentevent.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/pdusynchronize.h"
#include "pdu/pduchat.h"
#include "sys.h"
#include <math.h>

void CAction::tick(CTournament*, long now, long end) {}

CLimitAction::CLimitAction(const CChips& lolimit)
    :
    lolimit_(lolimit)
{}

void CLimitAction::fire(CTournament* t)
{
    //printf("CLimitAction: New limits: %d/%d\n", lolimit_, 2*lolimit_);
        
    char buf[128];
    sprintf(buf, "New limits: %f/%f" ,
           lolimit_.asDouble(), (2*lolimit_).asDouble());
    t->logEvent(TE_Tournament, NULL, buf);

    CpduTournamentParams pdu;
    pdu.sendTournamentParams(t->getTournamentQueue(),
                             TF_Limit,
                             lolimit_.getRep(),
                             (2*lolimit_).getRep());
}


CSyncAction::CSyncAction(SyncFlag sf)
    :
    syncFlag_ (sf),
    startTime_(0),
    mins_     (0),
    pauseTime_(0),
    statusSent_(false)
{}

CSyncAction::CSyncAction(SyncFlag sf, int pauseTime)
    :
    syncFlag_ (sf),
    startTime_(0),
    mins_     (0),
    pauseTime_(pauseTime),
    statusSent_(false)
{}

void CSyncAction::fire(CTournament* t)
{
    printf("*** CSyncAction::fire: %d\n", syncFlag_);

    if (syncFlag_ == SF_Pause)
    {
        t->setIsPaused(true);
        t->logEvent(TE_Tournament, NULL, "Pausing");
    }
    else if (syncFlag_ == SF_Play)
    {
        t->setIsPaused(false);
        t->logEvent(TE_Tournament, NULL, "Playing");
    }
    else
    {
        char s[100];
        sprintf(s, "Sync: %d", syncFlag_);
        t->logEvent(TE_Tournament, NULL, s);
    }

    CpduSynchronize pdu;
    pdu.sendSync(t->getTournamentQueue(),
                 syncFlag_);

    if (syncFlag_ == SF_Pause)
    {
        CpduTournamentParams pdu2;
        pdu2.sendTournamentParams(t->getTournamentQueue(),
                                  TF_Pause, pauseTime_, 0);
    }
}


void CSyncAction::tick(CTournament* t, long now, long end)
{
  if (syncFlag_ == SF_Play && !statusSent_)
  {
    if (startTime_ == 0)
      startTime_ = now;

    // Send status 1 minute after pause start
    if (now - startTime_ > 60)
    {
      statusSent_ = true;
      sendTournamentStatus(t);
    }
  }

  // If this is a pause action, then next sync action
  // will be a play
}

void CSyncAction::sendTournamentStatus(CTournament* t)
{
  CpduChat pdu;

  pdu.broadcastMessage("system", "Tournament status report:");
  Sys_Sleep(1);

  int numPlayersStarted = t->getNumPlayers();
  CChips cc_prize_pool = numPlayersStarted * t->getBuyin();
  if (t->getBuyin() == 0)
    cc_prize_pool = CChips(100, 0);
 
  char buf[1024];
  sprintf(buf, "%i/%i players left, prize pool: $%f",
          t->getNumPlayersPlaying(),
          t->getNumPlayers(),
          cc_prize_pool.asDouble());
  pdu.broadcastMessage("system", buf);
  Sys_Sleep(1);

  if (t->getBuyin() == 0)
  {
    pdu.broadcastMessage("system", "Top six will be awarded: $30, $15, $15, $15, $15, $10");
  }
  else
  {
    // XXX CCHIPS NOTE: cents not handled here
    float prize_pool = cc_prize_pool.getDollars();

    if (numPlayersStarted > 61)
    {
      float prize_percentages[] = {0.35f, 0.20f, 0.10f, 0.05f, 0.04f, 0.03333f, 0.03333f, 0.02666f, 0.02666f, 0.02333f,
                                   0.02f, 0.01666f, 0.01666f, 0.01333f, 0.01f, 0.01f, 0.01f, 0.00666f, 0.00666f, 0.00666f};
      sprintf(buf, "Positions one to ten will be awarded: $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i",
              (int)ceilf(prize_pool * prize_percentages[0]),
              (int)ceilf(prize_pool * prize_percentages[1]),
              (int)ceilf(prize_pool * prize_percentages[2]),
              (int)ceilf(prize_pool * prize_percentages[3]),
              (int)ceilf(prize_pool * prize_percentages[4]),
              (int)ceilf(prize_pool * prize_percentages[5]),
              (int)ceilf(prize_pool * prize_percentages[6]),
              (int)ceilf(prize_pool * prize_percentages[7]),
              (int)ceilf(prize_pool * prize_percentages[8]),
              (int)ceilf(prize_pool * prize_percentages[9]));
      pdu.broadcastMessage("system", buf);

      sprintf(buf, "Positions eleven to twenty will be awarded: $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i",
              (int)ceilf(prize_pool * prize_percentages[10]),
              (int)ceilf(prize_pool * prize_percentages[11]),
              (int)ceilf(prize_pool * prize_percentages[12]),
              (int)ceilf(prize_pool * prize_percentages[13]),
              (int)ceilf(prize_pool * prize_percentages[14]),
              (int)ceilf(prize_pool * prize_percentages[15]),
              (int)ceilf(prize_pool * prize_percentages[16]),
              (int)ceilf(prize_pool * prize_percentages[17]),
              (int)ceilf(prize_pool * prize_percentages[18]),
              (int)ceilf(prize_pool * prize_percentages[19]));
      pdu.broadcastMessage("system", buf);
    }
    else
    {
      float prize_percentages[] = {0.35f, 0.22f, 0.12f, 0.08f, 0.06f, 0.05f, 0.04f, 0.03f, 0.03f, 0.02f};
      sprintf(buf, "Positions one to ten will be awarded: $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i, $%i",
              (int)ceilf(prize_pool * prize_percentages[0]),
              (int)ceilf(prize_pool * prize_percentages[1]),
              (int)ceilf(prize_pool * prize_percentages[2]),
              (int)ceilf(prize_pool * prize_percentages[3]),
              (int)ceilf(prize_pool * prize_percentages[4]),
              (int)ceilf(prize_pool * prize_percentages[5]),
              (int)ceilf(prize_pool * prize_percentages[6]),
              (int)ceilf(prize_pool * prize_percentages[7]),
              (int)ceilf(prize_pool * prize_percentages[8]),
              (int)ceilf(prize_pool * prize_percentages[9]));
      pdu.broadcastMessage("system", buf);
    }
  }
}


CBetStructureAction::CBetStructureAction(int bet)
    :
    betStructure_(bet)
{}

void CBetStructureAction::fire(CTournament* t)
{
    printf("CBetStructureAction: Betting: %i\n", betStructure_);

    if (betStructure_ == BS_Limit)
        t->logEvent(TE_Tournament, NULL, "Limit");
    else if (betStructure_ == BS_PotLimit)
        t->logEvent(TE_Tournament, NULL, "Pot Limit");
    else if (betStructure_ == BS_NoLimit)
        t->logEvent(TE_Tournament, NULL, "No Limit");

    CpduTournamentParams pdu;
    pdu.sendTournamentParams(t->getTournamentQueue(),
                             TF_BetStructure,
                             betStructure_, 0);
}



CGameTypeAction::CGameTypeAction(GameType gt, bool hilo)
    :
    gameType_(gt),
    hilo_    (hilo)
{}

void CGameTypeAction::fire(CTournament* t)
{
    printf("CGameTypeAction: game: %i, hilo: %i\n", gameType_, hilo_);

    if (gameType_ == GT_Holdem)
        t->logEvent(TE_Tournament, NULL, "Holdem");
    else if (gameType_ == GT_Omaha)
        t->logEvent(TE_Tournament, NULL, "Omaha");
    else if (gameType_ == GT_SevenStud)
        t->logEvent(TE_Tournament, NULL, "7-Stud");

    CpduTournamentParams pdu;
    pdu.sendTournamentParams(t->getTournamentQueue(),
                             TF_GameType,
                             gameType_, hilo_ ? 1 : 0);
}

