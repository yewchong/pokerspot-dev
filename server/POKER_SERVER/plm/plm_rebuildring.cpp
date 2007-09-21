/**********************************************************************

  NAME:		plm_rebuildring.cpp
  DATE:		4/8/00
  PURPOSE:	Class Cplm implements the methods needed to actually run a
			poker game.  PLM stands for "Poker Logic Module", which is
			exactly what this class strives to implement: the logic to
			run a poker game.

**********************************************************************/

#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"
#include "pduplayer/pduplayer.h" 
#include "pdunotify/pdunotify.h" 
#include "pdulogoutconfirm/pdulogoutconfirm.h"
#include "pdupingpong/pduping.h" 
#include "lounge/loungeserver.h" 
#include "poller/poller.h"
#include "tournament/tournament.h"
#include "tournament/pdutournamentparams.h"

#define MAX_SITOUTS 13

//
// This member function rebuilds the table ring.
// Newly logged in players will be added to the ring
// and logged out players will be removed from the ring.
//
// NOTE: this code is protected with the same MUTEX
// that is used by CpduLogin and CpduLogout implementations.
// This ensures that the two threads never modify the
// ring uncontrollably.
//
void Cplm::rebuildRing()
{
    bool isTournament =
        CTournament::Inst()->isTournament();
    if (isTournament)
    {
        doTournament();
    }

    int numPlayers = table_->countPlayers(0);

    //
    // Destroy current ring
    //
    Cring* ring = table_->getRing();
    ASSERT(ring);
    Cring* next = ring->getNextNode();
    ring->wrapNode();
    ring->zeroPlayer();
    while (next != ring)
    {
        Cring* dead = next;
        next = next->getNextNode();
        delete dead;
    }

    if (checkShutdown())
        return; // shutting down - no more games

    //
    // Rebuild new ring of players ready to play,
    // and process players
    // - logging out
    // - all-in state
    // - in network error state
    //
    bool sendTableUpdate = false;
    int i = 0;
    for (i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* player = table_->getPlayerRawSlot(i);

        if (isTournament)
        {   // In tournaments the network error handling
            // is different
            handleNetworkErrors(player);
        }

        // Throw player out if he has sit out more
        // than MAX_SITOUTS consecutive hands

        // At N hands, give a warning
        if (player->getSitouts() == 10)
        {
            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getConnfd(),
                                  "You have been sitting out 10 consecutive hands.\n"
                                  "Please sit in or give your seat to other players.",
                                  NF_NoCloseConnection);
		    // make sure the message goes out only once.
		    player->setSitouts(player->getSitouts() + 1);
        }

        if (player->getSitouts() > (MAX_SITOUTS + 1) &&
            numPlayers > (table_->getMaxPlayers() / 2))
        {
            char s[200];
            sprintf(s, "Cplm::rebuildRing: Player %s sits out too long, will disconnect.", player->getUsername());
            Sys_LogError(s);

            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getConnfd(),
                                  "You have been sitting out 13 consecutive hands.\n"
                                  "Your seat will be now given to other players.");

            // log him out
            table_->savePlayerChips(player);

            CLoungeServer::SendPlayerUnseated(player);
   
            CpduPlayer plr(table_);
            plr.sendPlayerRaw(player, PLAYER_LOGOUT);
            CpduLogoutConfirm::SendLogoutConfirmPdu(player);

            CPoller::Instance()->removeClient(player->getConnfd());

            player->setUnused();
            sendTableUpdate = true;
        }

		bool wasAdded = false;

        if (player->isDisconnected()) 
        {   
            if (isTournament)
            {
                //
                // Mark disconnected user as zombie
                //
                player->setIsZombie(true);
                CpduPlayer pdu(table_);
                pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);

                // reset error state
                player->setDisconnected(false);
            }
            else
            {
                //
                // Log out disconnected user
                //
                CLoungeServer::SendPlayerUnseated(player);

                // Tell other players
                CpduPlayer pdu(table_);
                pdu.sendPlayerRaw(player, PLAYER_LOGOUT);

                // Save Player's chips!
                table_->savePlayerChips(player);
                player->setUnused();
                sendTableUpdate = true;
            }
        }
        else if (player->getSendError())
        {   
            if (isTournament)
            {
                //
                // Mark disconnected player as zombie
                //
                CPoller::Instance()->removeClient(player->getConnfd());
                player->setIsZombie(true);
                CpduPlayer pdu(table_);
                pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);
            
                // reset error state
                player->setSendError(false);
            }
            else
            {
                //
                // Log out disconnected user
                //
                CLoungeServer::SendPlayerUnseated(player);

                CpduNotify pdu;
                pdu.sendNotifyMessage(player->getConnfd(),
                                    "Your connection to the PokerSpot server is too unreliable.\n"
                                    "Your chips will be saved and the connection is closed.\n\n"
                                    "Please try reconnecting later.");

                CPoller::Instance()->removeClient(player->getConnfd());

                // Tell other players
                CpduPlayer plr(table_);
                plr.sendPlayerRaw(player, PLAYER_LOGOUT);
    
                // Save Player's chips!
                table_->savePlayerChips(player);
                player->setUnused();
                sendTableUpdate = true;
            }
        }
        else if (player->matchState(PLAYER_STATE_LOGGINGOUT))
        {
            if (isTournament)
            {
                // Mark player zombie
                player->setIsZombie(true);
                CpduPlayer pdu(table_);
                pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);
            }
            else
            {
                //
                // Finish the logout
                //
                CLoungeServer::SendPlayerUnseated(player);

                player->setUnused();
                sendTableUpdate = true;
            }
        }
        else if (player->matchState(PLAYER_STATE_ALLIN) ||
		         player->matchState(PLAYER_STATE_NOTRESPONDING))
        {
	      // This state means player has either gone
	      // - out-of-chips all-in
	      // - network timeout all-in during betting
	      // - network timeout during blind query

                //
                // Change all-in state to sitout - the player
                // will sit out the next hand but can join the
                // next hand after that if he has chips.
                // A player who has gone network all-in might
                // be able to rejoin the game when network stabilizes.
                //

            // Client's chip counts must be updated
            // when/if he rejoins
            player->setIsFirstHand(true);

            if (player->getChips() == 0)
            {
                if (isTournament)
                {
                    // In tournaments this is the only valid
                    // way for a player to leave the table
/*** XXX elimination order fix
                    CLoungeServer::SendPlayerUnseated(player);

                    CPoller::Instance()->removeClient(player->getConnfd());

                    // Tell other players
                    CpduPlayer plr(table_);
                    plr.sendPlayerRaw(player, PLAYER_LOGOUT);

                    player->setUnused();
***/
                    sendTableUpdate = true;
                }
                else
                {
                    // out-of-chips allin
                    player->stateChange(PLAYER_STATE_SITOUT);
                    CpduPlayer pdu(table_);
                    pdu.sendPlayerRaw(player, PLAYER_SITOUT);
                    CpduNotify n;
                    n.sendNotifyMessage(player->getConnfd(),
                                        "Please buy more chips to play, or log out",
                                        NF_StatusMessage|NF_NoCloseConnection);
                }
            }
            else
            {
                // network allin - the player will get out of
                // this state only after he has replied to a ping
                // successfully
                CpduPing p;
                if (player->getPings() > PING_MAX_FAILURES ||
                    p.sendPing(player) == -1)
                {
                    if (isTournament)
                    {
                        CPoller::Instance()->removeClient(player->getConnfd());
                        player->setIsZombie(true);
                        CpduPlayer pdu(table_);
                        pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);
                    }
                    else
                    {
                        CLoungeServer::SendPlayerUnseated(player);

                        char s[200];
                        sprintf(s, "Cplm::rebuildRing: Player %s fails ping test, will disconnect.", player->getUsername());
                        Sys_LogError(s);
    
                        CpduNotify pdu;
                        pdu.sendNotifyMessage(player->getConnfd(),
                                  "Your connection to the PokerSpot server is too unreliable.\n"
                                  "Your chips will be saved and the connection is closed.\n\n"
                                  "Please try reconnecting later.");

                        // he's really disconnected, log him out
                        table_->savePlayerChips(player);
                        CPoller::Instance()->removeClient(player->getConnfd());
    
                        CpduPlayer plr(table_);
                        plr.sendPlayerRaw(player, PLAYER_LOGOUT);
                        CpduLogoutConfirm::SendLogoutConfirmPdu(player);

                        player->setUnused();
                        sendTableUpdate = true;
                    }
                }
                else
                {
                    // send sit-out so clients don't show him
                    // as if sitting in
                    CpduPlayer pdu(table_);
                    pdu.sendPlayerRaw(player, PLAYER_SITOUT);
                }
            }
        }
        else if ((player->getConnfd() != -1) ||
                 (isTournament && !player->matchState(PLAYER_STATE_UNUSED)))
        {
            //
            // Rebuild the ring from those who are ready
            // to play
            //

            if (isTournament && player->getChips() == 0)
            {
/*** XXX elimination order fix
                CLoungeServer::SendPlayerUnseated(player);

                CPoller::Instance()->removeClient(player->getConnfd());

                // Tell other players
                CpduPlayer plr(table_);
                plr.sendPlayerRaw(player, PLAYER_LOGOUT);

                player->setUnused();
***/
                sendTableUpdate = true;
            }
            else
            {
                    
                // this rules out those who have logged in but
                // have not bought-in yet
                int state = PLAYER_STATE_SITTING & ~PLAYER_STATE_LOGGEDIN;
                if (player->matchState(state) ||
                    (isTournament && player->isZombie()))
                {
                    wasAdded = true;
                    ring->addPlayer(player);
                }
            }
        }

	// In tournaments player must be added to ring
	// in all cases
	if (!wasAdded && isTournament &&
	    !player->matchState(PLAYER_STATE_UNUSED) &&
	    player->getChips() != 0)
	  {
	    // Player must be in proper state when added to the ring
	    player->stateChange(PLAYER_STATE_WAITING);
	    ring->addPlayer(player);
	  }
    }

    // Elimination order fix:
    // Handle tournament player elimination
    if (isTournament)
    {
	    CTournament::Inst()->checkEliminated();
    }    

    // Send table update to lounge server if players were logged out
    if (sendTableUpdate && CLoungeServer::Inst())
        CLoungeServer::Inst()->sendTableUpdate(table_->countPlayersRaw());

    // Go through observers and throw out if there are errors
    for (i = 0; i < CTable::MaxObservers; i++)
    {
        CPlayer* observer = table_->getObserver(i);
        if (observer && (observer->getConnfd() != -1) && 
            observer->getSendError())
        {
            CPoller::Instance()->removeClient(observer->getConnfd());
            observer->setUnused();
        }
    }

    if (isTournament)
    {
        CChips lo = 2, hi = 4;
        CTournament::Inst()->getLimits(lo, hi);
        if (lo != table_->getLo() || hi != table_->getHi())
        {
            table_->setLo(lo);
            table_->setHi(hi);
            CpduTournamentParams pdu;
            pdu.sendParams(table_,
                           TF_Limit,
                           lo.getRep(),
                           hi.getRep());
            Sys_Sleep(10000);
        }

        static SyncFlag s = CTournament::Inst()->getSyncState();
        if (s != CTournament::Inst()->getSyncState())
        {
            s = CTournament::Inst()->getSyncState();

            // Notify lounge server that the sync
            // state has changed
            CpduSynchronize pdu;
            if (CLoungeServer::Inst())
	            pdu.sendSync(CLoungeServer::Inst()->getSocket(), s);
        }
    }

    if (isTournament)
    {
      CTournament::Inst()->makeEliminationOrder();
    }
    
    // XXX first hand
    sendChipUpdates();
}


void Cplm::doShutdown()
{
    // Save all player's chips if we have live db connection.
    // If the shutdown is because of a database problem, the
    // users have already been informed and it makes no sense
    // to try to save the chips to the database.
    if (table_->getDbase()->dbPing())
    {
        for (int i = 0; i < CTable::MaxPlayers; i++)
        {
            CPlayer* player = table_->getPlayerRawSlot(i);
            if (!player->matchState(PLAYER_STATE_UNUSED) && player->getChips() > 0)
            {
                // Save Player's chips!
                table_->savePlayerChips(player);
            }
        }

        if (table_->notifyShutdown())
        {
            string msg;
            table_->getDbase()->getShutdownMsg(msg);

            CpduNotify pdu;
            if (msg.size() != 0)
            {
                pdu.sendAllRaw(table_, msg.c_str());
            }
            else
            {
                pdu.sendAllRaw(table_,
                            "PokerSpot server is shutting down for maintenance.\n\n"
                            "Thank You for playing at PokerSpot!");
            }
        }
    }
}


// Send chip updates for newly logged in players.
// This fixes the problem of new players not having
// accurate latest state.
void Cplm::sendChipUpdates()
{
    CRingIterator it(table_->getRing());

    int state = PLAYER_STATE_WAITING|PLAYER_STATE_READY|PLAYER_STATE_PLAYING;
    if (it.start(state))
    {
        do
        {
            CPlayer* p = it.current()->getPlayer();
            if (p && p->isFirstHand())
            {
                sendChipUpdates(p);
                p->setIsFirstHand(false);
            }
        }
        while (it.next(state));
    }
}


void Cplm::sendChipUpdates(CPlayer* newPlayer)
{
    CpduPlayer pdu(table_);

    for (int i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* player = table_->getPlayerRawSlot(i);
        if (!player->matchState(PLAYER_STATE_UNUSED) &&
            player != newPlayer)
        {
            pdu.sendChipUpdate(newPlayer, player);
        }
    }
}

// Return true if shutting down.
bool Cplm::checkShutdown()
{
    static u_int16_t shutdown = 0;
    if (shutdown == 0)
    {   // This is done only once per shutdown
        shutdown = table_->checkShutdown();
        if (shutdown != 0 && table_->notifyShutdown())
        {   
            doShutdown();
        }
    }

    return (shutdown != 0);
}

void Cplm::doTournament()
{
    CTournament* tournament = CTournament::Inst();

    // If the tournament was frozen, notify clients
    static bool wasFrozen = tournament->isFrozen();
    if (!wasFrozen && tournament->isFrozen())
    {
        wasFrozen = true;
        CpduTournamentParams pdu;
        pdu.sendParams(table_,
                       TF_Freeze,
                       0, 0);
        table_->freezeTournament();
        Sys_Sleep(5000);
    }

    int gameType = tournament->getGameType();
    bool isHiLo = tournament->isHiLo();
    if (gameType != table_->getGameType() ||
        isHiLo != isHiLoSplit())
    {
        table_->setGameType(gameType);
        setHiLoSplit(isHiLo);

        // Recreate the game script object anew
        FILE* inFile = NULL;
        switch (table_->getGameType())
        {
        case (Game_Type_Holdem) : inFile = fopen(HOLDEM_FILE, "r"); break;
        case (Game_Type_SevenStud) : inFile = fopen(SEVENSTUD_FILE, "r"); break;
        case (Game_Type_Omaha) : inFile = fopen(OMAHA_FILE, "r"); break; 
        default: printf("GameType %d not supported!\n", table_->getGameType()); break;
        }

        if (inFile)
        {
            delete pgl_;
            pgl_ = new Cpgl(inFile);
            if (!pgl_)
            {
                ASSERT("Server out of memory!" == NULL);
            }

            fclose(inFile);
	    inFile = NULL;	    
        }

        CpduTournamentParams pdu;
        pdu.sendParams(table_,
                       TF_GameType,
                       gameType, isHiLo);
        Sys_Sleep(5000);
    }

    BetStructure bs = (BetStructure)tournament->getBettingStructure();
    if (bs != getBetStructure())
    {
        setBetStructure(bs);
        CpduTournamentParams pdu;
        pdu.sendParams(table_,
                       TF_BetStructure,
                       bs, 0);
        Sys_Sleep(5000);
    }

    // Check if this table is dissolved
    static bool dissolve = false;
    if (!dissolve && tournament->getDissolve())
    {   // This is one-time-only operation
        dissolve = true;
        tournament->dissolveTable();
    }

    // If the tournament is paused, notify clients
    if (tournament->pause())
    {
        u_int16_t pauseTime = tournament->getPauseTime();

        static long lastTime = time(NULL);
        long now = time(NULL);
        if (now - lastTime > 10)
        {
            lastTime = now;
            // Notify every 10 seconds
            CpduTournamentParams pdu;
            if (pauseTime != 0)
            {
                int elapsed = now - tournament->getPauseStartTime();
                if (pauseTime > elapsed)
                    pdu.sendParams(table_, TF_Pause,
                                   pauseTime - elapsed,
                                   0);
            }
            else
            {
                pdu.sendParams(table_, TF_Pause, 0, 0);
            }
        }        
    }
}


// In tournaments, turn disconnected players into zombies.
void Cplm::handleNetworkErrors(CPlayer* player)
{
    if (player->isDisconnected()) 
    {   
        player->setIsZombie(true);
        CpduPlayer pdu(table_);
        pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);

        // reset error state
        player->setDisconnected(false);
    }
    else if (player->getSendError())
    {   
        CPoller::Instance()->removeClient(player->getConnfd());
        player->setIsZombie(true);
        CpduPlayer pdu(table_);
        pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);
            
        // reset error state
        player->setSendError(false);
    }
    else if (player->matchState(PLAYER_STATE_LOGGINGOUT))
    {
        player->setIsZombie(true);
        CpduPlayer pdu(table_);
        pdu.sendPlayerRaw(player, PLAYER_ZOMBIE);
    }

    if (player->isZombie())
    {
        if (player->reanimate())
        {   // The player is turned into a non-zombie
            // here again.
            player->doReanimate();
        }
        else
            player->setIsZombie(true); // increment zombie counter
    }

    // XXX: it is possible for a player to be
    // non-zombie, disconnected and in PLAYER_STATE_WAITING
    // state (how?!). Take care of that by turning the
    // player into a zombie now.
    if (!player->isZombie() &&
        player->getConnfd() == -1 &&
        player->matchState(PLAYER_STATE_WAITING))
    {
        player->setIsZombie(true);
    }

    if (player->matchState(PLAYER_STATE_ZOMBIE))
    {
        // In tournaments zombies will be blinded off
        player->stateChange(PLAYER_STATE_WAITING);
    }
}

