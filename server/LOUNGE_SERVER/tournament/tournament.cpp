/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Tournament
  Created  : 07/05/2000

  OVERVIEW : Tournament support.

 ****************************************************************************/

#include "tournament/tournament.h"
#include "tournament/waitstartstate.h"
#include "tournament/basetournamentqueue.h"
#include "pdu/pduqueueupdate.h"
#include "dbhandler/dbinterface.h"
#include "sys.h"
#include "algorithm"


CTournament::CTournament(CBaseTournamentQueue* tq,
                         int number,
                         int maxPlayers,
                         const CChips& buyin,
                         const CChips& startChips,
                         const CChips& startLo,
                         u_int32_t     startTime,
                         const string& descr,
                         const string& script)
    :
    number_     (number),
    buyin_      (buyin),
    startChips_ (startChips),
    startLo_    (startLo),
    startTime_  (startTime),
    numPlayers_ (0),
    maxPlayers_ (maxPlayers),
    freezeTime_ (120), // 2 minutes by default
    gameType_   (GT_Holdem),
    isHilo_     (false),
    description_(descr),
    state_      (NULL),
    isRunning_  (false),
    isPaused_   (false),
    tournamentQueue_(tq),
    flipFlop_   (false)
{
  ASSERT(tq != NULL);

  if (startTime_ > time(NULL))
    state_ = new CWaitStartState(this);
}

CTournament::~CTournament()
{
  clearEvents();
  delete state_;
  state_ = NULL;
}


void CTournament::tick(long now)
{
  // Do once per two tics
  flipFlop_ = !flipFlop_;
  if (!flipFlop_) return;
  
  if (state_)
    state_->tick(now);
  else
  {
    if (startTime_ > now)
    {
      state_ = new CWaitStartState(this);
      setNumPlayers(0);
    }
  }
}


void CTournament::setNextState(CTournamentState* state)
{
  state_ = state;
}


string CTournament::getTitle() const
{
  char buf[16];
  sprintf(buf, "Tournament %i", number_);
  return string(buf);
}


u_int32_t CTournament::getStartTime() const
{
  return startTime_;
}


int CTournament::getNumPlayers() const
{
  if (numPlayers_ == 0)
  { // If we're not running yet,
    // return the number of players
    // in the queue
    int n = 0;

    CBaseTournamentQueue*
      tq = getTournamentQueue();
    if (tq)
    n = tq->getPlayers().size();

    return n;
  } 

  return numPlayers_;
}

int CTournament::getNumPlayersPlaying() const
{
    int nplayers = 0;

    if (numPlayers_ == 0)
    {   // Return number of players in queue
        nplayers = getNumPlayers();
    }
    else
    {   // Count actual number of players playing
        CBaseTournamentQueue*
            tq = getTournamentQueue();
        if (tq)
            nplayers = tq->countPlayers();
    }

    return nplayers;
}

void CTournament::setNumPlayers(int n)
{
    numPlayers_ = n;
}


bool CTournament::canAddPlayer(const CPlayer& p)
{
    bool rc = true;

    if (state_)
        rc = state_->canAddPlayer(p);

    return rc;
}


bool CTournament::logEvent(int eventId,
                           const char* player,
                           const char* extra)
{
    return CdbInterface::Inst()->logTournamentEvent(number_,
                                                    eventId,
                                                    time(NULL),
                                                    player,
                                                    extra);
}


void CTournament::playerReseat(CTable*     newTable,
                               const char* username)
{
    if (state_)
        state_->playerReseat(newTable, username);
}


void CTournament::waitReseat(const list<string>& players)
{
    if (state_)
        state_->waitReseat(players);
}


bool CTournament::isUnfrozen() const
{
    bool rc = false;
    if (state_)
        rc = state_->isUnfrozen();
    return rc;
}


bool CTournament::allowRelogin() const
{
    bool rc = false;
    if (state_)
        rc = state_->allowRelogin();
    return rc;
}

void CTournament::dumpEvents()
{
    printf("CTournament events: %d %d %d\n",
           timedEvents_.size(), playerEvents_.size(), tableEvents_.size());
}


namespace
{

template <class T>
struct Deallocator : public unary_function<void, T*>
{
  void operator()(T* t)
  {
    delete t;
  }
};

}

void CTournament::clearEvents()
{
  // Deallocate all events
  for_each(timedEvents_.begin(), timedEvents_.end(), Deallocator<CTimedEvent>());
  for_each(playerEvents_.begin(), playerEvents_.end(), Deallocator<CPlayerEvent>());
  for_each(tableEvents_.begin(), tableEvents_.end(), Deallocator<CTableEvent>());

  // Clear the lists
  timedEvents_.clear();
  playerEvents_.clear();
  tableEvents_.clear();
}


bool CTournament::tableDissolveInProgress() const
{
  return state_ && state_->tableDissolveInProgress();
}