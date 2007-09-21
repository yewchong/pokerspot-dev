/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPlayer
  Created  : 1/1/2001

  OVERVIEW : This class represents a player at a table.

 ****************************************************************************/


#include "player.h"
#include "pduhandler/pduhandler.h"
#include "pdupingpong/pduping.h"
#include "ipcheck.h"
#include "lounge/loungeserver.h"
#include "tournament/tournament.h"
#include "ssl_funcs.h"

#define MAX_TIMEOUTS 2

// Initialize as unused player
CPlayer::CPlayer(int slot)
    :
    connfd_      (-1),
//    chips_       (0),
    state_       (PLAYER_STATE_UNUSED),
    slot_        (slot),
    currentBet_  (0),
    muckCards_   (false),
    sitIn_       (true),
    disconnected_(false),
    sitouts_     (0),
    pings_       (0),
    sendError_   (false),
    ipaddr_      (0),
    isFreeSeat_  (false),
    isZombie_    (0),
    reanimate_   (false),
    timeouts_    (0),
    isFirstHand_ (true),
#pragma warning (disable : 4355) // harmless: 'this' used in ctor
    pingCounter_ (this)
#pragma warning (default : 4355)
{
    memset(username_, 0, sizeof(username_));
    memset(password_, 0, sizeof(password_));
    memset(city_, 0, sizeof(city_));
    hand_ = new Chand();
}


CPlayer::~CPlayer()
{   
  if (hand_)
  {
    hand_->free();
    delete hand_;
    hand_ = NULL;
  }
}

void CPlayer::printPlayer()
{
  printf("Player Name: %s, Password: %s, Chips: %d.%d\n",
         username_, password_, chips_.getDollars(), chips_.getCents());
};

void CPlayer::stateChange(int state)
{
    if (state == PLAYER_STATE_LOGGINGOUT)
    {   // If the player is logging out, clear socket
        // so it'll be ignored by pdu sending loops
        connfd_ = -1;
    }
    if (state_ == PLAYER_STATE_LOGGINGOUT) 
    {
        // The logging out state is not allowed to be overwritten
        // by other states except (1) PLAYER_STATE_LOGGININ and
        // (2) PLAYER_STATE_ZOMBIE.
        // (1) will be set when a new player logs in and takes the
        // structure into use. The logic thread will occasionally
        // try to overwrite LOGGINOUT state, this here prevents it.
        // (2) is used in tournaments when a player logs out - he
        // will be marked a zombie and will be blinded off
        if (state != PLAYER_STATE_LOGGEDIN &&
            state != PLAYER_STATE_ZOMBIE)
            return;
    }

    state_ = state;
}

void CPlayer::setDisconnected(bool b)
{
    disconnected_ = b;
    if (b)
    {
        connfd_ = -1;
    }
}

bool CPlayer::matchConnfd(int connfd)
{
    if (connfd == connfd_)
        return true;
    else
        return false;
};

bool CPlayer::matchState(int state)
{
    if (state == 0 || (state & state_))
        return true;
    else
        return false;
};

bool CPlayer::matchSlot(int slot)
{
    if (slot == slot_)
        return true;
    else
        return false;
};

bool CPlayer::matchUsername(const char *username)
{
    int i;

    for (i = 0; i < PDUSTRINGSIZE; i++)
    {
        if (username[i] != username_[i])
            return false;
    }

    return true;
}


//
// MFUNCTION: CPlayer::getFromQueue
// 
// PURPOSE: Pop first pdu from player's queue.
// 
// RETURNS: The first pdu in the queue or
//          NULL if the queue is empty.
//
// NOTE: The caller must deallocate the
//       returned pdu.
//
Cpdu* CPlayer::getFromQueue()
{
    Cpdu* pdu = NULL;
    SingleLock l(&pduMutex_);
    if (l.lock())
    {
        if (pduQueue_.size() == 0)
            return NULL;

        PduQueue::iterator it = pduQueue_.begin();

        pdu = *it;
        pduQueue_.erase(it);
    }
    else
    {
        Sys_LogError("CPlayer::getFromQueue: can't lock pduMutex_");
    }

    return pdu;
}

//
// MFUNCTION: CPlayer::queuePdu
// 
// PURPOSE: Push pdu to player's pdu queue.
//
bool CPlayer::queuePdu(Cpdu* pdu)
{
    bool rc = false;

    SingleLock l(&pduMutex_);
    if (l.lock())
    {
        pduQueue_.push_back(pdu);

        // Set the pdu event so the logic thread
        // will be invoked
        pduEvent_.setEvent();

        rc = true;
    }
    else
    { 
        rc = false;
        Sys_LogError("CPlayer::queuePdu: can't lock pdu queue.\n");
    }

    return rc;
}

//
// MFUNCTION: CPlayer::waitPdu
// 
// PURPOSE: Wait for an incoming pdu for the
// specified timeout.
// If the incoming pdu has a sequence number different
// from what is expected, the pdu is ignored.
//
// RETURN:
// If a pdu arrives in time, return 0, otherwise
// return Pdu_Wait_Timeout.
//
int CPlayer::waitPdu(Cpdu** ppPdu, DWORD timeOut, u_int16_t sequenceNumber)
{
    int rc = 0;

    if (disconnected_)
        return CPlayer::Pdu_Wait_Timeout;

    long start = 0, elapsed = 0;
    time(&start);    

    // note: timeOut is in milliseconds while
    // elapsed is in seconds

    while (timeOut > (1000 * elapsed))
    {
        if (pduEvent_.lock(timeOut - (1000 * elapsed)))
        {   // When we get here we have an incoming PDU
            pduEvent_.resetEvent();

            // Get all pdus from queue, consider the one
            // with correct sequence number
            while (Cpdu* pdu = getFromQueue())
            {
                // if sequence numbers don't match, ignore
                // the pdu and wait for next one to arrive
                if (checkSequenceNumber(pdu, sequenceNumber))
                {
                    // XXX Tournament: pdu received, reset timeouts
                    setTimeouts(0);
                    *ppPdu = pdu;
                    return 0; // OK!
                }
                else
                {
                    // pdu had wrong sequence number - it is
                    // and old pdu and is ignored
                    delete pdu;

                    // The loop will check for more pdus and
                    // will resume waiting if necessary
                }
            }

            long now = 0;
            time(&now);
            elapsed += (now - start);
        }
        else
        {
            // Wait timed out, will exit loop
            elapsed = (1000 * timeOut);
        }
    }

    // this is ok, player did not respond within timeout
    printf("CPlayer::waitPdu: timed out (%s)\n", getUsername());
    rc = CPlayer::Pdu_Wait_Timeout;

    // XXX Tournament
    if (CTournament::Inst()->isTournament())
    {
        CpduPing p;
        if (p.sendPing(this) == -1)
        {
          char buf[200];
          sprintf(buf, "Player %s sendPing==-1\n", getUsername());
          Sys_LogError(buf);
          setSendError(true);
        }

        setTimeouts(getTimeouts() + 1);
        if (getTimeouts() >= MAX_TIMEOUTS)
        {
            // Player is not responding - set network error flag
            char buf[200];
            sprintf(buf, "Player %s (%d) times out\n",
                    getUsername(), getConnfd());
            Sys_LogError(buf);

            setSendError(true);
        }
    }

    return rc;
}


bool CPlayer::checkSequenceNumber(Cpdu* pdu, u_int16_t sequenceNumber)
{
    bool rc = false;
    char s[200];

    u_int16_t sqn = pdu->getSequenceNumber();
    if (sqn == PDU_NOT_SEQUENCED)
    {
        sprintf(s, "CPlayer::waitPdu: non-sequenced pdu type (%s, %d)\n",
                getUsername(), pdu->getType());
        Sys_LogError(s);
        rc = false;
    }
    else if (sqn != sequenceNumber)
    {   // This is ok: it can happen if there is a peek in latency
        // that causes a pdu to travel for so long the server times
        // out and the pdu arrives after that. The pdu will be ignored
        // by the caller.
        sprintf(s, "CPlayer::waitPdu: wrong sequence number for pdu (%s, expected %d got %d)\n",
                getUsername(), sequenceNumber, sqn);
        Sys_LogError(s);
        rc = false;
    }
    else
    {
        // ok, sequence number matches
        rc = true;
    }

    return rc;
}

void CPlayer::setPlayer(int connfd,
                        const char* username,
                        const char* password,
                        const CChips& chips,
                        u_int32_t ipaddress)
{
    strncpy(username_, username, PDUSTRINGSIZE);
    strncpy(password_, password, PDUSTRINGSIZE);
    memset(city_, 0, sizeof(city_));

    if (chips == 0 || isZombie())
    {   // zombie or tournament free seat =>
        // this seat/player already has chips assigned
    }
    else
    {
        chips_ = chips;
    }

    // XXX Does not work - in normal tournament logins
    // the socket must be set immediately!
    // XXX Zombie: when player reanimates zombie,
    // new socket is saved to temporary variable.
    // Cplm::rebuildRing will then take the new
    // socket into use. 
    connfd_ = connfd;

    if (state_ == PLAYER_STATE_UNUSED)
        state_ = PLAYER_STATE_LOGGEDIN; 
    disconnected_ = false;
    sitouts_ = 0;
    pings_ = 0;
    sendError_ = false;
    ipaddr_ = ipaddress;
    if (ipaddr_ != 0)
        RegisterIPAddress(ipaddr_);

    // XXX Fix tournament double big blind bug
    if (!CTournament::Inst()->isTournament())
        missedBlind_ = 0;

    isFirstHand_ = true;
}

void CPlayer::setCity(const char* city)
{
    if (city != NULL && *city != '\0')
        strncpy(city_, city, PDUSTRINGSIZE);
}

void CPlayer::setUnused()
{
  pingCounter_.reset();
  
  if (chips_ != 0)
  {
    char s[200];
    if (username_[0] != '\0')
    {
      sprintf(s, "NOTE: Player %s leaves, %d chips left\n",
              username_, chips_.getDollars());
    }
    else
    {
      sprintf(s, "NOTE: Player at slot %d leaves, %d chips left\n",
              slot_, chips_.getDollars());
    }
    // In tournaments this is ok
    if (!CTournament::Inst()->isTournament())
      Sys_LogError(s);
  }
  
  // Forget about this ip address
  if (ipaddr_ != 0)
    UnregisterIPAddress(ipaddr_);
  
  connfd_ = -1;
  chips_ = 0;
  state_ = PLAYER_STATE_UNUSED;
  memset(username_, 0, sizeof(username_));
  memset(password_, 0, sizeof(password_));
  memset(city_, 0, sizeof(city_));
  
  muckCards_ = false;
  sitIn_ = true;
  disconnected_ = false; 
  missedBlind_ = 0;
  sitouts_ = 0;
  pings_ = 0;
  sendError_ = false;
  ipaddr_ = 0;
  
  isFreeSeat_ = false;
  isZombie_ = 0;
  reanimate_ = false;
  timeouts_ = 0;
  
  // Empty player's pdu queue!
  while (Cpdu* pdu = getFromQueue())
  {
    delete pdu;
  }
}

Chand* CPlayer::getHand()
{
  if (hand_ == NULL)
  {
    hand_ = new Chand();
  }

  return hand_;
}

void CPlayer::killHand()
{
  if (hand_ != NULL)
  {
    hand_->free();
    hand_ = NULL;
  }
};


CChips CPlayer::getMissedBlind() const
{
  return missedBlind_;
}


bool CPlayer::isFirstHand() const
{
  return isFirstHand_;
}


void CPlayer::setIsFirstHand(bool b)
{
  isFirstHand_ = b;
}

void CPlayer::setIsZombie(bool b)
{
  if (b)
  {
    stateChange(PLAYER_STATE_ZOMBIE);
    
    // When the player is marked a zombie,
    // the socket must have been closed already!
    //ASSERT(!ssl_checksocket(connfd_));
    connfd_ = -1;
  }
  
  if (b)
    isZombie_++;
  else
    isZombie_ = 0;
}

// A zombie is reanimated only at Cplm::rebuildRing.
// This is to prevent the newly logged in player from
// getting in in the middle of a hand which the client
// does not handle very well.
void CPlayer::doReanimate()
{
  isZombie_ = false;
  reanimate_ = false;
  timeouts_ = 0;
}

bool CPlayer::reanimate() const
{
    return reanimate_;
}

int CPlayer::getTimeouts() const
{
    return timeouts_;
}

void CPlayer::setTimeouts(int i)
{
    timeouts_ = i;
}
