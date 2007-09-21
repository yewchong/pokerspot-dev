/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournament
  Created  : 07/11/2000

  OVERVIEW : Tournament manager class for table server.

 ****************************************************************************/

#include "tournament/tournament.h"
#include "tournament/tournamentevent.h"
#include "tournament/pdudissolveinfo.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/pduplayerfinished.h"
#include "lounge/loungeserver.h"
#include "table/table.h"
#include "poller/poller.h"
#include "pdunotify/pdunotify.h" 
#include "pduplayer/pduplayer.h" 
#include "dbinterface/dbinterface.h"
#include "plm/plm.h"
#include "sys.h"
#include <algorithm>

#define NCTHIS const_cast<CTournament*>(this)

static CTournament g_Tournament;
CTournament* CTournament::pInst_ = NULL;


CTournament* CTournament::Inst()
{
  return pInst_;
}

void CTournament::getLimits(CChips& lolimit, CChips& hilimit) const
{
  SingleLock l(&(NCTHIS->mutex_));
  l.lock();

  lolimit = lolimit_;
  hilimit = hilimit_;
}

u_int16_t CTournament::getGameType() const
{
  return gameType_;
}

void CTournament::setLimits(const CChips& lolimit, const CChips& hilimit)
{
  SingleLock l(&(NCTHIS->mutex_));
  l.lock();

  lolimit_ = lolimit;
  hilimit_ = hilimit;
}

void CTournament::setGameType(u_int16_t gameType)
{
  gameType_ = gameType;
}

u_int16_t CTournament::getBettingStructure() const
{
  return betStructure_;
}

void CTournament::setBettingStructure(u_int16_t b)
{
  betStructure_ = b;
}

bool CTournament::pause() const
{
  return isTournament_ && syncState_ == SF_Pause;
}

CTournament::CTournament()
  :
  isTournament_(false),
  dissolve_    (false),
  number_      (0),
  numPlayers_  (1),
  lolimit_     (DEFAULT_LOW),
  hilimit_     (DEFAULT_HI),
  gameType_    (Game_Type_Holdem),
  betStructure_(Cplm::BS_Limit),
  hilo_        (false),
  pauseTime_   (0),
  pauseStart_  (0),
  syncState_   (SF_Pause),
  isFrozen_    (false)
{
  pInst_ = this;
}


bool CTournament::logEvent(int eventId,
                           const char* player,
                           const char* extra)
{
  return false;
}

void CTournament::playerFinished(CPlayer* player)
{
  CTable* table = CTable::Instance();
  char buf[100];
  sprintf(buf, "Table %i chips %9.2f",
          table->getSeed(), player->getChips().asDouble());

  int position = 0;
  bool rc = table->getDbase()->logPlayerLeave(
                                      number_,
                                      time(NULL),
                                      player->getUsername(),
                                      buf,
                                      position);

  player->setChips(CChips(0, 0));
  int pos = numPlayers_ + 1 - position;

  if ((player->getConnfd() != -1))
  {
    CpduNotify pdu;
    char s[200];

    if (pos <= 8)
    {
      sprintf(s, "Congratulations! You finished in position %i out of %i players.",
              pos, numPlayers_);
    }
    else
    {
      sprintf(s, "You finished in position %i out of %i players. Better luck next time!",
              pos, numPlayers_);
    }

    pdu.sendNotifyMessage(player->getConnfd(), s);
  }

  CpduPlayerFinished pdu2;
  pdu2.sendPlayerFinished(table,
                          player,
                          0xffff,
                          pos,
                          numPlayers_);
}

// Dissolving means we just send the dissolve info
// pdu back to the lounge server. Lounge server will
// then reseat the players.
void CTournament::dissolveTable()
{
  // Tell players
  CpduTournamentParams p1;
  p1.sendParams(CTable::Instance(),
                TF_Dissolve, 0, 0);

  // Send dissolve info back to lounge server
  CpduDissolveInfo p2;
  p2.sendDissolveInfo(CLoungeServer::Inst()->getSocket(),
                      CTable::Instance());
}

void CTournament::declareWinner(CTable* table)
{
    SingleLock l(&table->getTableMutex());
    l.lock();

    CPlayer* winner = NULL;

    for (int i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* player = table->getPlayerRawSlot(i);
        if (player && !player->matchState(PLAYER_STATE_UNUSED))
        {
            winner = player;
            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getConnfd(),
                                  "Congratulations!\n\nYou have won the tournament!");

            char buf[100];
            sprintf(buf, "WINNER: %s", player->getUsername());
            Sys_LogTournament(buf);

            break;
        }
    }

    CpduNotify pdu;
    if (winner)
    {
        char buf[100];
        sprintf(buf, "%s wins the tournament!", winner->getUsername());
        pdu.sendObservers(table, buf);
    }
    else
    {
        pdu.sendObservers(table, "This table is shutting down.");
    }
}

void CTournament::setPauseTime(u_int16_t t)
{
    pauseTime_ = t;
    pauseStart_ = time(NULL);
}

void CTournament::setTournamentNumber(int n)
{
    if (n != number_)
    {
        char buf[100];
        sprintf(buf, "tlog-%i-%i.txt",
                n, CTable::Instance()->getSeed());
        Sys_InitTournamentLog(buf);
        sprintf(buf, "Table %i tournament %i",
                CTable::Instance()->getSeed(), n);
        Sys_LogTournament(buf);
    }
    number_ = n;
}


// SORT ON ASCENDING ORDER
struct SortOnChips
{
    bool operator()(CPlayer* lhs, CPlayer* rhs) const
    {
        return lhs->getChips() < rhs->getChips();
    }
};

void CTournament::makeEliminationOrder()
{
    // Build list of players in order in which they'll
    // be eliminated if they go out in the next hand. The
    // order is based on the number of the chips the players
    // have at the moment.

    eliminationOrder_.clear();

    for (int i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* player = CTable::Instance()->getPlayerFromSlot(i);
        if (player && !player->isFreeSeat())
            eliminationOrder_.push_back(player);
    }

    sort(eliminationOrder_.begin(), eliminationOrder_.end(), SortOnChips());

/* print out the elimination order
    printf("Elimination list order:\n");
    for (vector<CPlayer*>::iterator it = eliminationOrder_.begin(),
                                    end = eliminationOrder_.end();
         it != end;
         ++it)
    {
        CPlayer* player = (*it);
        printf("\t%s %i\n", player->getUsername(), player->getChips());
    }
*/
}

int CTournament::checkEliminated()
{
    int numEliminated = 0;
    CTable* table = CTable::Instance();

    for (vector<CPlayer*>::iterator it = eliminationOrder_.begin(),
                                    end = eliminationOrder_.end();
         it != end;
         ++it)
    {
        CPlayer* player = (*it);

        if (player->getChips() == 0)
        {
            numEliminated++;
            CLoungeServer::SendPlayerUnseated(player);
            CPoller::Instance()->removeClient(player->getConnfd());
            // Tell other players
            CpduPlayer plr(table);
            plr.sendPlayerRaw(player, PLAYER_LOGOUT);
            player->setUnused();
        }
    }

    return numEliminated;
}