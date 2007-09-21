/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CTable
  Created  : 1/1/2001

  OVERVIEW : This class maintains the state of a poker table.

 ****************************************************************************/


#include "global.h"
#include "gettime.h"
#include "table/table.h"
#include "ipcheck.h"
#include "pdulogin/pdulogin.h"
#include "pdulogin/pduobserverlogin.h"
#include "pdulogin/pduloginreject.h"
#include "pdulogout/pdulogout.h"
#include "pdulogout/pduobserverlogout.h"
#include "pdulogoutconfirm/pdulogoutconfirm.h"
#include "pduplayer/pduplayer.h"
#include "pduplayerinfo/pduplayerinfo.h"
#include "pdubuyinquery/pdubuyinquery.h"
#include "pdubuyinrequest/pdubuyinrequest.h"
#include "pdunotify/pdunotify.h"
#include "pduplay/pdudealcommunity.h"
#include "player/player.h"
#include "ring/ring.h"
#include "poller/poller.h"
#include "dbinterface/dbinterface.h"
#include "lounge/loungeserver.h"
#include "gamemain.h"
#include "plm/plm.h"
#include "tournament/tournament.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/pduclientreseat.h"
#include "common/chips.h"
#include "common/stringoutput.h"


// This will allow direct logins not coming from 
// the lounge server.
#define ALLOW_DIRECT_LOGINS_ 1

extern ThreadInfo g_ThreadInfo;
bool g_bIsPersistent = false; // persistent table impl

// Class Method Definitions Follow

CTable* CTable::pInstance_ = NULL;

CTable::CTable()
  :
  gameType_(Game_Type_Holdem),
  seed_    (0),
  hi_      (DEFAULT_HI),
  low_     (DEFAULT_LOW),
  port_    (0),
  connfd_  (0),
  buttonslot_(0),
  flopprcnt_(0),
  potavg_   (0),
  totalPots_(0),
  numPots_  (0),
  tableMax_ (MaxPlayers),
  activePot_(NULL),
  bet_      (0),
  ring_     (NULL),
  gameNumber_(0),
  shutdown_ (0),
  notifyShutdown_(true)
{
  ring_ = new Cring();
  board_ = new Chand();
  activePot_ = &pot_;
  deck_.initDeck();
  dbase_ = NULL;
  pInstance_ = this;

  memset(players_, 0, sizeof(players_));
  memset(observers_, 0, sizeof(observers_));

  // Allocate all player structures and mark
  // them unused
  for (int i = 0; i < MaxPlayers; i++)
  {
    players_[i] = new CPlayer(i);
    if (players_[i])
      players_[i]->stateChange(PLAYER_STATE_UNUSED);
    else
    {
      Sys_LogError("CTable::CTable: out of memory!\n");
      break;
    }
  }
}

CTable::~CTable()
{
    pInstance_ = NULL;

    delete ring_;
    ring_ = NULL;

    int i = 0;

    for (i = 0; i < MaxPlayers; i++)
    {
        delete players_[i];
        players_[i] = NULL;
    }

    for (i = 0; i < MaxObservers; i++)
    {
        delete observers_[i];
        observers_[i] = NULL;
    }

    if (board_)
        board_->free();
}


#ifdef DEBUG
void CTable::printTable()
{
  // Print the table's properties
  printf("GameType: %d\nSeed: %d\n", gameType_, seed_);

  deck_.printDeck();
}
#endif


CPlayer* CTable::getPlayerFromSlot(int slot)
{
  return ring_->getPlayerFromSlot(slot);
}

CPlayer* CTable::getPlayerFromSocket(SOCKET sd)
{
  return ring_->getPlayer(sd);
}

CdbInterface* CTable::getDbase()
{
  return dbase_;
}

int CTable::countPlayers(int state)
{
  if (!ring_)
    return 0;
  return ring_->countPlayers(state);
}

void CTable::setConnfd(unsigned int val)
{
  connfd_ = val;
}

void CTable::setBet(const CChips& bet)
{
  bet_ = bet;
}

void CTable::setButtonSlot(unsigned int buttonslot)
{
  buttonslot_ = buttonslot;
}

bool CTable::advanceButton()
{
  unsigned int newbutton = buttonslot_;
  
  // In 7-stud there really is no button.
  // We take care of it by having a static button always
  // at last seat.
  if (gameType_ == Game_Type_SevenStud)
    newbutton = 9;
  else
    newbutton = (buttonslot_ + 1) % 10;
  
  while (!players_[newbutton]->matchState(PLAYER_STATE_READY|
          PLAYER_STATE_WAITING))
  {
    if (gameType_ == Game_Type_SevenStud)
    {
      if (newbutton == 0)
        newbutton = 9;
      else
        newbutton--;
    }
    else
    {            
      newbutton = (newbutton + 1) % 10;
    }
    if (newbutton == buttonslot_)
    {
      return false;       // No players are eligible for the button!
    }
  }
  
  // A player eligible for the button was found!
  buttonslot_ = newbutton;
  
  return true;
}

void CTable::setDbase(CdbHandler *dbase)
{
  dbase_ = new CdbInterface(dbase);
}

void CTable::setGameType(unsigned int val)
{
  gameType_ = (Game_Type)val;
}

void CTable::setHi(const CChips& val)
{
  hi_ = val;
}

void CTable::setLo(const CChips& val)
{
  low_ = val;
}

void CTable::setPort(unsigned int val)
{
  port_ = val;
}

void CTable::setSeed(unsigned int val)
{
  seed_ = val;
}

void CTable::tableLogin(SOCKET sd, CpduLogin* loginPdu)
{
  bool loginOk = true;
  u_int32_t ipaddr = 0;

  // Is there a valid login entry for the player? If not,
  // he could be trying to log in directly and not via lounge
  LoginEntries::iterator pos = loginEntries_.find(loginPdu->getUsername());
  if (pos == loginEntries_.end())
  {
#ifdef ALLOW_DIRECT_LOGINS_
#pragma message("Allowing direct logins!!!")

    // WILL ALLOW DIRECT LOGINS

#else
    char s[200];
    sprintf(s, "Login rejected: no login entry for user %s", loginPdu->getUsername());
    Sys_LogError(s);
    loginOk = false;
#endif
  }
  else
  {
    ipaddr = (*pos).second.ipaddr_;
    // player is logging in, remove his entry
    loginEntries_.erase(pos);
  }

  // In theory we didn't need to do these checks here
  // because they're done when the lounge server asks
  // whether the login is ok. Do the check just in case
  // someone finds a way to log in to table without
  // the lounge server.
  if (loginOk)
    loginOk = checkLogin(ipaddr, loginPdu->getUsername());

  if (!loginOk || !dbase_->authenticate(loginPdu->getUsername(), loginPdu->getPassword()))
  {
    // Send Table Reject PDU
    CpduLoginReject pdu;
    pdu.sendReject(sd);
    // Terminate the connection
    CPoller::Instance()->removeClient(sd);
  }
  else
  {
    CPlayer* newPlayer = NULL;

    // Add player to table
    SingleLock l(&tableMutex_);
    if (l.lock())
    {
      newPlayer = getUnusedPlayer(loginPdu->getUsername());
      if (newPlayer)
      {
				// In tournaments there is a small window during which a
        // player may succeed logging in twice because the first
        // login does not "reanimate the zombie" immediately.
        // This check prevents it from happening.
				if (CTournament::Inst()->isTournament() &&
				    checkDoubleLogin(loginPdu->getUsername(), newPlayer))
				{
					// Send Table Reject PDU
					CpduLoginReject pdu;
					pdu.sendReject(sd);
					// Terminate the connection
					CPoller::Instance()->removeClient(sd);
					return;
				}

        newPlayer->setPlayer(sd, 
                             loginPdu->getUsername(),
                             loginPdu->getPassword(),
                             CChips(), // 0 chips until he buys in
                             ipaddr);

        newPlayer->setIsFreeSeat(false);
        newPlayer->setReanimate(true);

        string city;
        if (dbase_->getCity(loginPdu->getUsername(), city))
        {
          newPlayer->setCity(city.c_str());
        }

        if (countPlayers(PLAYER_STATE_PLAYING|PLAYER_STATE_WAITING) >= 1 &&
            countPlayersRaw(false) > 2 &&
            !CTournament::Inst()->isTournament()) // XXX Fix tournament double big blind bug
        {
          // a game is in progress - must charge 'new player's blind'
          newPlayer->setMissedBlind(getLo());
        }

        CpduPlayerInfo myPlayerInfoPdu(this);
        CpduBuyinQuery myBuyinQueryPdu(this);

        // Send Player Info PDU
        myPlayerInfoPdu.sendPlayerInfo(newPlayer); 

        if (CTournament::Inst()->isTournament())
        {
          char buf[100];
          sprintf(buf, "Player %s logged in",
                  newPlayer->getUsername());
          Sys_LogTournament(buf);

          CTournament* t = CTournament::Inst();
          CChips lo(DEFAULT_LOW), hi(DEFAULT_HI);
          t->getLimits(lo, hi);

          // Send current game type & limits
          CpduTournamentParams params;
          params.sendParams(newPlayer, TF_GameType,
                            t->getGameType(),
                            t->isHiLo() ? 1 : 0);
          params.sendParams(newPlayer, TF_Limit,
                            lo.getRep(),
                            hi.getRep());

          // In tournament there is automatic buy-in when
          // players are seated
          if (newPlayer->matchState(PLAYER_STATE_UNUSED|PLAYER_STATE_LOGGEDIN))
              newPlayer->stateChange(PLAYER_STATE_BOUGHTIN); 
        }
        else
        {
          // Send buy in query
          myBuyinQueryPdu.sendBuyinQuery(newPlayer);
        }

        // Send table update to lounge server
        if (CLoungeServer* ls = CLoungeServer::Inst())
        {
          ls->sendPlayerSeated(newPlayer->getUsername());
          ls->sendTableUpdate(countPlayersRaw());
        }
      }
    }

    if (!newPlayer)
    {   // no room for player!
      CPoller::Instance()->removeClient(sd);
    }
  }
}


void CTable::tableLogout(SOCKET sd, CpduLogout* /*logoutPdu*/)
{
    bool isTournament = CTournament::Inst()->isTournament();

    SingleLock l(&tableMutex_);
    if (l.lock())
    {
        CPlayer* deadPlayer = getPlayerRawSocket(sd);
        if (deadPlayer)
        {
            printf("Logging out player %s\n", deadPlayer->getUsername());

            if (isTournament)
            {   // In tournament he will become a zombie
                ;
            }
            else
            {   
                savePlayerChips(deadPlayer);

                CpduPlayer pdu(this);
                pdu.sendPlayerRaw(deadPlayer, PLAYER_LOGOUT);
            }

            CpduLogoutConfirm::SendLogoutConfirmPdu(deadPlayer);
            deadPlayer->stateChange(PLAYER_STATE_LOGGINGOUT);

            CPoller::Instance()->removeClient(sd);

            // Lounge server will be notified when the player
            // slot is freed in CPlm::rebuildRing()
        }
        else
        {   // XXX: in an early observer implementation
            // the client accidentally sends table logout pdu
            // instead of the observer logout pdu. Handle it
            // here just in case.
            CPlayer* deadObserver = NULL;
            for (int i = 0; i < MaxObservers; i++)
            {
                if (observers_[i] != NULL &&
                    observers_[i]->getConnfd() == sd)
                {
                    deadObserver = observers_[i];
                    break;
                }
            }

            if (deadObserver)
            {
                printf("Logging out observer %s\n", deadObserver->getUsername());
                CpduLogoutConfirm::SendLogoutConfirmPdu(deadObserver);
            }       
            CPoller::Instance()->removeClient(sd);

            if (deadObserver)
            {
                deadObserver->stateChange(PLAYER_STATE_UNUSED);
                deadObserver->setUnused();
            }
        }
    }

    // This is done in CPoller::removeClient
//    Sys_close(sd);

    return;
}

CPlayer* CTable::getUnusedPlayer(const char* username)
{
    // First, reuse player's zombie if possible
    if (username && *username != '\0')
    {
        CPlayer* p = findZombie(username);
        if (p)
            return p;
    }

    bool freeSeat = false;
    // XXX Tournament
    if (CTournament::Inst()->isTournament())
    {
        // Do we have any "Open Seat" fake
        // players? If we do, use one.
        int slot = 0;
        while (slot < 10)
        {
            if (players_[slot]->isFreeSeat())
            {
                freeSeat = true;
                break;
            }
            slot++;
        }
    }


    if (getGameType() == Game_Type_SevenStud)
    {
        // Try a random slot a few times, if no
        // luck resort to first free seat
        for (int i = 0; i < 10; i++)
        {
            int slot = rand() % MaxPlayers;
            if (slot == 1 || slot == 8)
                slot++;
            if ((!freeSeat && players_[slot]->matchState(PLAYER_STATE_UNUSED)) ||
                (freeSeat && players_[slot]->isFreeSeat())) // XXX Tournament
            {
                return players_[slot];
            }
        }


        // Find the first free slot
        int slot = 0;
    
        while (slot < 10)
        {
            if (slot == 1 || slot == 8)
                slot++;
    
            if ((!freeSeat && players_[slot]->matchState(PLAYER_STATE_UNUSED)) ||
                (freeSeat && players_[slot]->isFreeSeat())) // XXX Tournament
            {
                return players_[slot];
            }
            slot++;
        }

        return NULL;    
    }
    else
    {

        // Try a random slot a few times, if no
        // luck resort to first free seat
        for (int i = 0; i < 10; i++)
        {
            int slot = rand() % MaxPlayers;
            if ((!freeSeat && players_[slot]->matchState(PLAYER_STATE_UNUSED)) ||
                (freeSeat && players_[slot]->isFreeSeat())) // XXX Tournament
            {
                return players_[slot];
            }
        }
    

        // Find the first free slot
        int slot = 0;
    
        while (slot < 10)
        {
            if ((!freeSeat && players_[slot]->matchState(PLAYER_STATE_UNUSED)) ||
                (freeSeat && players_[slot]->isFreeSeat())) // XXX Tournament
                return players_[slot];
            slot++;
        }

        return NULL;
    }
}

void CTable::newDeck()
{
    deck_.freeDeck();
    deck_.initDeck();
}

void CTable::shuffleDeck()
{
    deck_.shuffleDeck();
}

void CTable::dealCards(CPlayer *player, int numcards)
{
    CCard *tmpCard;
    CpduDealCardsFlex pdu;
    char message[100] = { '\0' };
    char vChar[3] = { '\0' };
    char sChar;

    if (numcards)
        sprintf(message, "%s was dealt ", player->getUsername());

    pdu.setDeal(0xffff, Hole);
    
    for (; numcards > 0; numcards--)
    {
        tmpCard = deck_.popCard();

        player->getHand()->addCard(tmpCard);

        memset(vChar, 0x0, sizeof(vChar));
        
        switch (tmpCard->getValue())
        {
        case (10):  strcpy(vChar, "10"); break;
        case (11):  vChar[0] = 'J'; break;
        case (12):  vChar[0] = 'Q'; break;
        case (13):  vChar[0] = 'K'; break;
        case (14):  vChar[0] = 'A'; break;
        default: itoa(tmpCard->getValue(), vChar, 10); break;
        }

        switch (tmpCard->getSuit())
        {
        case (1):   sChar = 'C'; break;         // Clubs
        case (2):   sChar = 'D'; break;         // Diamonds
        case (3):   sChar = 'H'; break;         // Hearts
        case (4):   sChar = 'S'; break;         // Spades
        }
        
        sprintf(message, "%s [%s%c]", message, vChar, sChar);

        pdu.addCard(tmpCard->getValue(), tmpCard->getSuit());

        delete tmpCard;
    }

    pdu.sendDealCards(player);

    setLog(message);
  
}


void CTable::dealUpCards(CPlayer *player, int numcards)
{
    CCard *tmpCard;
    CpduDealCardsFlex pdu;
    char message[100] = { '\0' };
    char vChar[3] = { '\0' };
    char sChar;

    if (numcards)
        sprintf(message, "%s was dealt ", player->getUsername());

    pdu.setDeal(player->getSlot(), Hole);
    
    for (; numcards > 0; numcards--)
    {
        tmpCard = deck_.popCard();

        player->getHand()->addCard(tmpCard);

        memset(vChar, 0x0, sizeof(vChar));
        
        switch (tmpCard->getValue())
        {
        case (10):  strcpy(vChar, "10"); break;
        case (11):  vChar[0] = 'J'; break;
        case (12):  vChar[0] = 'Q'; break;
        case (13):  vChar[0] = 'K'; break;
        case (14):  vChar[0] = 'A'; break;
        default: itoa(tmpCard->getValue(), vChar, 10); break;
        }

        switch (tmpCard->getSuit())
        {
        case (1):   sChar = 'C'; break;         // Clubs
        case (2):   sChar = 'D'; break;         // Diamonds
        case (3):   sChar = 'H'; break;         // Hearts
        case (4):   sChar = 'S'; break;         // Spades
        }
        
        sprintf(message, "%s [%s%c]", message, vChar, sChar);

        pdu.addCard(tmpCard->getValue(), tmpCard->getSuit());

        delete tmpCard;
    }

    pdu.sendAllDealCards(getRing());

    setLog(message);  
}



void CTable::dealCards(Cring *ring, int numcards)
{
    CCard *tmpCard;
    CpduDealCommunity pdu;
    char message[100] = "Community Cards dealt ";
    char vChar[3] = { '\0' };
    char sChar;

    

    for (; numcards > 0; numcards--)
    {
        tmpCard = deck_.popCard();

        memset(vChar, 0x0, sizeof(vChar));

        switch (tmpCard->getValue())
        {
        case (10):  strcpy(vChar, "10"); break;
        case (11):  vChar[0] = 'J'; break;
        case (12):  vChar[0] = 'Q'; break;
        case (13):  vChar[0] = 'K'; break;
        case (14):  vChar[0] = 'A'; break;
        default: itoa(tmpCard->getValue(), vChar, 10); break;
        }

        switch (tmpCard->getSuit())
        {
        case (1):   sChar = 'C'; break;         // Clubs
        case (2):   sChar = 'D'; break;         // Diamonds
        case (3):   sChar = 'H'; break;         // Hearts
        case (4):   sChar = 'S'; break;         // Spades
        }

        getBoard()->addCard(tmpCard);

        sprintf(message, "%s [%s%c]", message, vChar, sChar);
        pdu.sendDealCommunity(ring, tmpCard->getValue(), tmpCard->getSuit(), 500);
        delete tmpCard;
    }

    setLog(message);
}

void CTable::killPots()
{
    // first, walk through the list of pots, deleting them all.
    Cpot *pot = &pot_;
    Cpot *tmpPot;
    
    tmpPot = pot->getNext();

    while (tmpPot != NULL)
    {
        pot = tmpPot->getNext();
        delete tmpPot;
        tmpPot = pot;
    }

    // now bzero the main pot
    // XXX it is not a good idea to memset an
    // object to 0 - if the class has virtual functions
    // memset will zero out the vtable as well and the
    // next virtual function call will crash.
    // 
    //memset(&pot_, 0x0, sizeof(pot_));
    pot_.clear();
}


Cpot* CTable::getLivePot()
{
    // returns the last pot which is the live pot
    Cpot* pot = &pot_;
    
    while (pot->getNext() != NULL)
    {
        pot = pot->getNext();
    }

    return pot;
}

void CTable::killHands()
{
    // walk through our ring, calling player->getHand()->free() on each.
    Cring *pRing = ring_;

    if (board_ != NULL)
    {
        board_->free();
        board_ = NULL;
    }

    if (pRing == NULL)
    {
        return;
    }
    else
    {
        if (pRing->getPlayer() == NULL)
        {
            return;
        }
        else
        {
            pRing->getPlayer()->killHand();
        }
    }

    do
    {
        pRing = pRing->getNextNode();
        pRing->getPlayer()->killHand();
    } while(pRing != ring_);

}

bool CTable::openLog()
{
  return (dbase_->openLog());
}

bool CTable::setLog(const char* message)
{
  return (dbase_->setLog(gameNumber_, message));
}

bool CTable::flushLog()
{
  return (dbase_->flushLog(gameNumber_));
}

void CTable::setGameNumber()
{
  CStrOut message;
  gameNumber_ = dbase_->getGameNum();
  dbase_->createGameEntry(gameNumber_);

  bool isHiLo = g_ThreadInfo.plm_ &&
                g_ThreadInfo.plm_->isHiLoSplit();

  message << "Game #" << gameNumber_
          << " - " << low_ << '/' << hi_;

  switch (gameType_)
  {
  case (Game_Type_Holdem):
    message << " Texas Hold'em ";
    break;
    
  case (Game_Type_SevenStud):
    message << " 7-Card Stud ";
    break;
    
  case (Game_Type_Omaha):
    message << " Omaha ";
    break;
    
  default:
    message << " Unknown GameType ";
    break;
  }
  
  if (isHiLo)
    message << " Hi/Lo ";
  
  message << " - " << GetTime();
  
  if (CTournament::Inst()->isTournament())
    message << "Tournament #" << CTournament::Inst()->getTournamentNumber() << '\n';
  
  dbase_->openLog();
  dbase_->setLog(gameNumber_, message.str());
}

Cring *CTable::getDealer()
{
    if (gameType_ == Game_Type_SevenStud)
    {   // Dealer is always the player with the highest seat -
        // that way the first player to receive cards is the
        // player with the lowest seat
        Cring* r = ring_;
        Cring* dlr = NULL;
        int best = -1;

        while (r)
        {
            if (r->getPlayer()->getSlot() > best)
            {
                best = r->getPlayer()->getSlot();
                dlr = r;
            }
            r = r->getNextNode();

            if (r == ring_)
                break;
        }

        return dlr;
    }
    else
    {
        Cring *rPtr = ring_;

        while ((rPtr->getPlayer()->getSlot() != buttonslot_) && (rPtr->getNextNode() != ring_))
        {
            rPtr = rPtr->getNextNode();
        }

        if (rPtr->getPlayer()->getSlot() == buttonslot_)
            return rPtr;
        else
            return NULL;
    }
}

Chand *CTable::getBoard()
{
    if (board_ == NULL)
    {
        board_ = new Chand();
    }

    return board_;
}

void CTable::distributeBoard()
{
    if (!board_) return;

    CPlayer *player;
    CRingIterator it(ring_);
    CCard *pCard;

    // all-in players must be evaluated too
    int state = PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN;

    if (it.start(state))
    {
        do {
            player = it.current()->getPlayer();
            // if player has been all-in for longer
            // than a complete hand, he doesn't have cards
            if (player->hasHand())
            {
                pCard = board_->getCard();
                while (pCard != NULL)
                {
                    player->getHand()->addCard(pCard);
                    pCard = pCard->getNext();
                }
            }
        } while (it.next(state));
    }
}

CPlayer* CTable::getPlayerRawSocket(int sd) const
{
    for (int i = 0; i < MaxPlayers; i++)
    {
        if (players_[i]->getConnfd() == sd)
            return players_[i];
    }
    return NULL;
}

int CTable::countPlayersRaw(bool countZombies) const
{
    int numPlayers = 0;
    for (int i = 0; i < MaxPlayers; i++)
    {
        if (players_[i]->getConnfd() != -1 ||
            (countZombies && players_[i]->isZombie()))
        {
            numPlayers++;
        }
    }
    return numPlayers;
}

bool CTable::savePlayerChips(CPlayer* player)
{
    // XXX Tournament
    if (CTournament::Inst()->isTournament())
    {   // Don't save tournament chips!
        return true;
    }

    bool rc = dbase_->saveChips(player, player->getChips());

    if (rc)
    {
        player->setChips(0);
    }
    else
    {
        // Failed to save chips to database - what now??
        char s[100];
        sprintf(s, "Can't save player %s's chips: %d.",
                player->getUsername(), player->getChips());
        Sys_LogError(s);
    }

    return rc;
}

void CTable::disconnectPlayer(SOCKET sd)
{
    if (DEBUG & DEBUG_TABLE)
        printf("CTable.disconnectPlayer() called.\n");

    SingleLock l(&tableMutex_);
    if (l.lock())
    {
        CPlayer* deadPlayer = getPlayerRawSocket(sd);
        if (deadPlayer)
        {
            char s[200];
            sprintf(s, "Player %s is disconnected, chips: %d\n",
                    deadPlayer->getUsername(), deadPlayer->getChips());
            Sys_LogError(s);

            deadPlayer->setDisconnected(true);
        }
        else
        {
            // Check if it's one of the observers
            for (int i = 0; i < MaxObservers; i++)
            {
                if (observers_[i] != NULL &&
                    observers_[i]->getConnfd() == sd)
                {
                    observers_[i]->setUnused();
                }
            }
        }

        // Remove the socket from the listen set
        if (sd != -1)
          CPoller::Instance()->removeClient(sd);
    }
}

CChips CTable::getAveragePotSize() const
{
  return potavg_;
}

u_int16_t CTable::getFlopPercentage() const
{
  return flopprcnt_;
}

void CTable::updatePotAverage()
{
  // Total chips in this round
  CChips currentPot = pot_.getTotalChips();

  // Sum of totals
  totalPots_ += currentPot;

  numPots_++;
  potavg_ = (totalPots_ / numPots_);

  // Divide by two every now and then to prevent
  // overflow
  if (totalPots_ > USHRT_MAX)
  {
    numPots_ /= 2;
    totalPots_ /= 2;
  }
}

void CTable::updateFlopPercentage()
{
    int all = countPlayers(PLAYER_STATE_ACTIVE|PLAYER_STATE_SITOUT);
    if (all != 0)
    {
        flopprcnt_ = (100 * countPlayers(PLAYER_STATE_PLAYING)) / all;
    }
}

bool CTable::checkLogin(u_int32_t ipaddr, const char* username)
{
    if (shutdown_)
        return false; // shutting down!

    // XXX zombie
    if (findZombie(username))
        return true; // ok, will reanimate zombie!

    // Check that the same ip address is not logged in already!
    if (ipaddr != 0 && FindIPAddress(ipaddr))
    {
        char s[200];
        sprintf(s, "Rejecting login: ip address %ld (%s) already logged in.\n",
		        ipaddr, username);
        Sys_LogError(s);
        return false;
    }

    // Does the table have room?
    // Note: must take soon-to-log-in player into account here!
    // Otherwise logins could end up rejected later.
    bool rc = true;
    if (!CTournament::Inst()->isTournament())
        rc = (countPlayersRaw() + loginEntries_.size() < getMaxPlayers());

    if (!rc)
    {
        char s[200];
        sprintf(s, "Rejecting login for (%s): table is full.\n",
                username);
        Sys_LogError(s);
    }

    // Check name uniqueness
    if (rc)
    {
        // Check the player's name against existing players
        for (int i = 0; i < CTable::MaxPlayers; i++)
        {
            CPlayer* player = getPlayerRawSlot(i);
            if (player->getConnfd() != -1)
            {
#ifdef _WIN32
                if (strcmpi(username, player->getUsername()) == 0)
#else
                if (strcasecmp(username, player->getUsername()) == 0)
#endif
                {   // Found the same name!
                    char s[100];
                    sprintf(s, "checkLogin: Player %s already logged in\n",
                            username);
                    Sys_LogError(s);
                    rc = false;
                    break;
                }
            }
        }
    }

    return rc;
}


// The table will leave after this number of
// seconds if there has been no players
#define NOPLAYERS_TIMEOUT 60

u_int16_t CTable::getShutdown()
{
    if (shutdown_ == 0 && !CTournament::Inst()->isTournament())
    {
        // Check no-players exit condition:
        // If there are no players for 60 ticks,
        // set shut down
        static int ticks = 0;
        if (!g_bIsPersistent && (countPlayersRaw() + loginEntries_.size() == 0))
        {
            ticks++;
            if (ticks > NOPLAYERS_TIMEOUT)
            {
                // No players for N ticks, leave
                shutdown_ = 5;
            }
        }
        else
        {
            ticks = 0;
        }
    }
    return shutdown_;
}

u_int16_t CTable::checkShutdown()
{
    if (shutdown_ == 0)
        dbase_->getShutdown(shutdown_);
    return shutdown_;
}

//
// To do an emergency shutdown (because of database failure),
// call this mfunction with non-zero argument. 
// This function will inform players, dump current state to file
// and initiate the shutdown.
//
void CTable::setShutdown(u_int16_t shutdown, bool saveLog)
{
    if (saveLog && 
        shutdown != 0 &&  // this check takes care that the shutdown..
        shutdown_ == 0)   // procedure below is done only once
    {   
        char buf[200];
        sprintf(buf, "shutdownlog-%i.txt", seed_);
        FILE* fp = fopen(buf, "a+");
        if (fp)
        {
            fprintf(fp, "Server shutdown at %s", Sys_GetDate());
            fprintf(fp, "Players:\n");
            for (int i = 0; i < MaxPlayers; i++)
            {
                CPlayer* player = players_[i];
                if (player->getConnfd() != -1)
                {
                    fprintf(fp, "   %s chips: %d\n",
                            player->getUsername(),
                            player->getChips());
                }
            }

            fprintf(fp, "\nLast hand log:\n"); 
            dbase_->dumpHandlog(fp);

            fclose(fp);
            fp = NULL;
        }

        CpduNotify pdu;
        pdu.sendAllRaw(this, "The server is shutting down.\n\n"
                             "The current hand will be aborted and chips in pot will be given back to players.\n");
    }

    // Players will be notified if this is an
    // emergency shutdown
    notifyShutdown_ = saveLog;

    shutdown_ = shutdown;
}


void CTable::tick(long now)
{
    LoginEntries::iterator it = loginEntries_.begin();

    // Go through login entries, purge old
    while (it != loginEntries_.end())
    {
        if (now - (*it).second.timer_ > LOGIN_EXPIRE_TIME)
        {
            char s[200];
            if ((*it).first.size() > 0)
                sprintf(s, "Login entry for %s expired.", (*it).first.c_str());
            else
                sprintf(s, "Login entry for <no name> expired.");
            Sys_LogError(s);

            LoginEntries::iterator dead(it);
            ++it;
            loginEntries_.erase(dead);
        }
        else
        {
            ++it;
        }
    }
}

void CTable::addLoginEntry(const char* username, u_int32_t ipaddress)
{
    long now = 0;
    time(&now);

    string str(username);
    LoginEntries::value_type v(pair<const string, LoginEntry>(str, LoginEntry(now, ipaddress)));
    loginEntries_.insert(v);
}

bool CTable::gameHistoryUpdate(CPlayer *player)
{
    return dbase_->gameHistoryUpdate(player, gameNumber_);
}

bool CTable::putHandLogRequest(SOCKET sd, const char *email, int numhands)
{
    // XXX: Hand logs for 7-Stud not yet implemented
    if (gameType_ == Game_Type_SevenStud)
        return false;

    int actualnum;

    if (numhands)
    {
        actualnum = 5;
    } else {
        actualnum = 1;
    }
 
    CPlayer* player = getPlayerRawSocket(sd);

    if (player)
        return dbase_->putHandLogRequest(player, email, actualnum);
    else
        return false;
}


bool CTable::canObserve(u_int32_t ipaddr, const char* username)
{
    SingleLock l(&tableMutex_);
    l.lock();

    bool rc = false;

    // If there is a free observer slot, allow observing
    for (int i = 0; i < MaxObservers; i++)
    {
        if (observers_[i] == NULL ||
            observers_[i]->matchState(PLAYER_STATE_UNUSED))
        {
            rc = true;
            break;
        }
    }

    // Check name uniqueness
    if (rc)
    {
        // Check the player's name against existing observers
        for (int i = 0; i < CTable::MaxObservers; i++)
        {
            CPlayer* player = observers_[i];
            if (player && player->getConnfd() != -1)
            {
#ifdef _WIN32
                if (strcmpi(username, player->getUsername()) == 0)
#else
                if (strcasecmp(username, player->getUsername()) == 0)
#endif
                {   // Found the same name!
                    rc = false;
                    break;
                }
            }
        }
    }

    return rc;
}

void CTable::observerLogin(SOCKET sd, CpduObserverLogin* pdu)
{
    SingleLock l(&tableMutex_);
    l.lock();

    bool loginOk = canObserve(0, pdu->getUsername());
    if (!loginOk || !dbase_->authenticate((char*)pdu->getUsername(),
                                          (char*)pdu->getPassword()))
    {
        if (DEBUG & DEBUG_TABLE)
            printf("Failed observer login attempt from %s, using password %s!\n",
                pdu->getUsername(), pdu->getPassword());

        // Send Table Reject PDU
        CpduLoginReject pdu;
        pdu.sendReject(sd);

        // Terminate the connection
        CPoller::Instance()->removeClient(sd);
//        Sys_close(sd);
    }
    else
    {
         CPlayer* newObserver = NULL;

        // Find first unused slot
        for (int i = 0; i < MaxObservers; i++)
        {
            if (observers_[i] == NULL ||
                observers_[i]->matchState(PLAYER_STATE_UNUSED))
            {
                if (observers_[i] == NULL)
                    observers_[i] = new CPlayer(0);
                newObserver = observers_[i];
                break;
            }
        }

        if (!newObserver)
        {
            // No room for observer
            if (DEBUG & DEBUG_TABLE)
                printf("Failed observer login attempt from %s, using password %s!\n",
                       pdu->getUsername(), pdu->getPassword());

            // Send Table Reject PDU
            CpduLoginReject pdu;
            pdu.sendReject(sd);

            // Terminate the connection
            CPoller::Instance()->removeClient(sd);
            return;
        }

       if (DEBUG & DEBUG_TABLE)
            printf("Observer %s logged in successfully.\n",
                pdu->getUsername());

        newObserver->setPlayer(sd,
                               pdu->getUsername(),
                               pdu->getPassword(),
                               0, 0);

        newObserver->stateChange(PLAYER_STATE_OBSERVING);

        CpduPlayerInfo myPlayerInfoPdu(this);

        // Send Player Info PDU
        myPlayerInfoPdu.sendObserverInfo(newObserver);

        if (CTournament::Inst()->isTournament())
        {
            CTournament* t = CTournament::Inst();
            CChips lo(DEFAULT_LOW), hi(DEFAULT_HI);
            t->getLimits(lo, hi);

            // Send current game type & limits
            CpduTournamentParams params;
            params.sendParams(newObserver, TF_GameType,
                              t->getGameType(),
                              t->isHiLo() ? 1 : 0);
            params.sendParams(newObserver, TF_Limit,
                              lo.getRep(),
                              hi.getRep());
        }
    }
}

void CTable::observerLogout(SOCKET sd, CpduObserverLogout*)
{
    SingleLock l(&tableMutex_);
    l.lock();

    if (DEBUG & DEBUG_TABLE)
        printf("CTable.observerLogout() called.\n");

    // Find the observer

    CPlayer* deadObserver = NULL;

    for (int i = 0; i < MaxObservers; i++)
    {
        if (observers_[i] != NULL &&
            observers_[i]->getConnfd() == sd)
        {
            deadObserver = observers_[i];
            break;
        }
    }

    if (deadObserver)
    {
        printf("Logging out observer %s\n", deadObserver->getUsername());
        CpduLogoutConfirm::SendLogoutConfirmPdu(deadObserver);
    }
        
    CPoller::Instance()->removeClient(sd);

    if (deadObserver)
      {
	deadObserver->stateChange(PLAYER_STATE_UNUSED);
	deadObserver->setUnused();
      }

    return;
}


Cring* CTable::getRingNode(CPlayer* p) const
{
    Cring* nextNode = ring_;
    if (!nextNode)
        return NULL;

    do
      {
        CPlayer* player = nextNode->getPlayer();

        if (player == p)
            return nextNode;

	      nextNode = nextNode->getNextNode();
      }
    while (nextNode != ring_);

    return NULL;
}

bool CTable::seatPlayer(const char* username, const CChips& chips, bool checkZombie)
{
  printf("CTable::seatPlayer %s %9.2f %i\n", username, chips.asDouble(), checkZombie);
  
  SingleLock l(&tableMutex_);
  l.lock();
  
  bool rc = false;
  bool setMissedBlind = false; 
  
  CPlayer* player = NULL;
  if (checkZombie)
  {
    player = findZombie(username);
    if (!player)
    {
      CPlayer* testPlayer = findPlayer(username);
      int setSendErr = 0;
      if (testPlayer)
      {
        setSendErr = 1;
        if (testPlayer->getTimeouts() > 0)
        {
          setSendErr = 2;
          if (!testPlayer->getSendError())
          {
            // Mark him disconnected right away
            testPlayer->setSendError(true);
            setSendErr = 3;
          }
        }
      }
      
      char buf[200];
      sprintf(buf, "No zombie for player %s found (%i)\n", username, setSendErr);
      Sys_LogError(buf);
      return false;
    }
  }
  else
  {
    // Check name uniqueness!
    if (findPlayer(username))
      return false;
    
    player = getUnusedPlayer(NULL);
    if (player)
      printf("Found an unused player!\n");
    else
      printf("No unused player found!\n");
    
    // XXX Fix tournament double big blind bug
    // This is a reseat! Must mark him as having missed
    // blind to ensure he won't be seated between small
    // and big blind to prevent double big blinds in
    // tourneys
    setMissedBlind = true;
  }
  
  if (player)
  {   // Must not change state if player is relogging
    // in after becoming disconnected.
    if (player->matchState(PLAYER_STATE_UNUSED) ||
        player->isFreeSeat())
    {
      player->setPlayer(0,        // no socket until he logs in
                        username,
                        "",
                        chips,    // tournament chips
                        0);
      
      player->stateChange(PLAYER_STATE_BOUGHTIN);
      player->setIsZombie(true);
      player->setIsFreeSeat(false);
    }
    addLoginEntry(username, 0);
    
    CpduPlayerInfo pdu(this);
    pdu.sendPlayerInfo(player);
    
    rc = true;
  }
  
  // XXX Fix tournament double big blind bug
  if (setMissedBlind && player)
  {
    printf("Setting missed blind for %s\n", player->getUsername());
    player->setMissedBlind(1);
  }
  
  return rc;
}


// Fill remaining unused seats. These seats will have the
// same amount of tournament chips and they will be blinded off
void CTable::fillSeats(const CChips& chips)
{
  printf("CTable::fillSeats\n");

  SingleLock l(&tableMutex_);
  l.lock();

  int numPlayers = tableMax_ - countPlayersRaw();

  for (int i = 0; (i < MaxPlayers) && (numPlayers > 0); i++)
  {
    if (players_[i]->matchState(PLAYER_STATE_UNUSED))
    {
      char buf[32];
      sprintf(buf, "Open Seat %i", i + 1);
      players_[i]->setPlayer(0,
                             buf,
                             buf,
                             chips,
                             0);
      players_[i]->stateChange(PLAYER_STATE_BOUGHTIN);
      players_[i]->setIsZombie(true);
      players_[i]->setIsFreeSeat(true);

      CpduPlayerInfo pdu(this);
      pdu.sendPlayerInfo(players_[i]);

      numPlayers--;
    }
  }
}


// Remove fake players in unused seats.
void CTable::freezeTournament()
{
    printf("CTable::freezeTournament\n");

    SingleLock l(&tableMutex_);
    l.lock();

    for (int i = 0; i < MaxPlayers; i++)
    {
        if (players_[i]->isFreeSeat())
        {
            CpduPlayer plr(this);
            plr.sendPlayerRaw(players_[i], PLAYER_LOGOUT);
            players_[i]->setUnused();
            players_[i]->setIsFreeSeat(false);
        }
    }
}


// If player with 'username' exists and he is a zombie,
// return the player ptr.
CPlayer* CTable::findZombie(const char* username)
{
    SingleLock l(&tableMutex_);
    l.lock();

    for (int i = 0; i < MaxPlayers; i++)
    {
#ifdef _WIN32
        if (strcmpi(username, players_[i]->getUsername()) == 0)
#else
        if (strcasecmp(username, players_[i]->getUsername()) == 0)
#endif
        {
            if (players_[i]->isZombie())
                return players_[i];
            else
                return NULL;
        }
    }

    return NULL;
}


void CTable::reseatPlayer(const char* username, u_int32_t addr, u_int16_t port)
{
    SingleLock l(&tableMutex_);
    if (l.lock())
    {
        CPlayer* player = findPlayer(username);

        if (player && player->getChips() > 0)
        {
            CpduClientReseat pdu;
            pdu.sendReseat(player->getConnfd(),
                           addr, port);
        }
    }
}


CPlayer* CTable::findPlayer(const char* username)
{
    for (int i = 0; i < MaxPlayers; i++)
    {
#ifdef _WIN32
        if (strcmpi(username, players_[i]->getUsername()) == 0)
#else
        if (strcasecmp(username, players_[i]->getUsername()) == 0)
#endif
        {
            return players_[i];
        }
    }

    return NULL;
}


bool CTable::checkDoubleLogin(const char* username, CPlayer* testPlayer)
{
  // Find player with 'username'. If it is not the same as
  // 'testPlayer', we have a double login and it must be rejected
  
  for (int i = 0; i < MaxPlayers; i++)
    {
#ifdef _WIN32
      if (strcmpi(username, players_[i]->getUsername()) == 0)
#else
        if (strcasecmp(username, players_[i]->getUsername()) == 0)
#endif
	  {
            if (players_[i] != testPlayer)
	      return true;
	  }
    }
  
  // Another check: a player might just have been eliminated
  // from the tournament. As it takes a moment for the information
  // to go to the lounge server, there is a small window during
  // which the player might succeed in logging in again. Prevent
  // this by checking if he's been eliminated.
  if (CTournament::Inst()->isFrozen())
    { 
      if (testPlayer && 
	  testPlayer->getChips() == 0)
	{
	  char buf[100];
	  sprintf(buf, "TOURNAMENT: %s already eliminated\n",
		  username);
	  Sys_LogError(buf);
	  return true;
	}
    }
  
  return false;
}


void CTable::resetMissedBlinds()
{
  for (int i = 0; i < MaxPlayers; i++)
  {
    if (players_[i])
      players_[i]->setMissedBlind(0);
  }
}


int CTable::numPlayersSittingIn() const
{
  int c = 0;

  for (int i = 0; i < MaxPlayers; i++)
  {
    if (players_[i])
    {
      if (!players_[i]->matchState(PLAYER_STATE_UNUSED) &&
          players_[i]->getSitIn())
      {
        c++;
      }
    }
  }

  return c;
}
