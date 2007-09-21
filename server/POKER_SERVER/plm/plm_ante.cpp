/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : plm_ante.cpp
  Created  : 1/1/2001

  OVERVIEW : Ante related Cplm mfunctions.

 ****************************************************************************/

#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "pot/pot.h"
#include "player/player.h"
#include "pduplay/pdublindrequest.h"
#include "pduplay/pdupostblind.h"
#include "pduplay/pdusitinquery.h"
#include "pduplay/pdusitin.h"
#include "pduplay/pdusitout.h"
#include "pduplay/pduactionecho.h"
#include "pduplay/pdubutton.h"
#include "pduplay/pduannounce.h"
#include "pduplayer/pduplayer.h"
#include "pdunotify/pdunotify.h"
#include "tournament/tournament.h"
#include "tournament/pduanteallin.h"
#include "common/stringoutput.h"

void GetBlinds(const CChips& currentBet,
               CChips& smBlind,
               CChips& bigBlind)
{
  switch (currentBet.getDollars())
  {
  case 15:    // rule: 15/30 limits, 10/15 blinds
    smBlind = CChips(10, 0);
    bigBlind = CChips(15, 0);
    break;
    
  default:
    smBlind = currentBet / 2;
    bigBlind = currentBet;
    break;
  }
}

void Cplm::doAnte()
{
    bool isTournament =
        CTournament::Inst()->isTournament();

    CChips currentBet = table_->getLo();
    CChips smBlind, bigBlind, toPay;

    GetBlinds(currentBet, smBlind, bigBlind);

    // Remember the players who refused button so
    // they won't be asked twice for sit-in
    // NOTE: This would be a whole lot simpler if the
    // button player didn't have to sit-in
    int buttonRefused[10];
    memset(buttonRefused, 0, sizeof(buttonRefused));

    // Players in these states can join next hand
    int state = PLAYER_STATE_WAITING|PLAYER_STATE_READY; 

    CPlayer* player = decideButtonPlayer(buttonRefused);
    if (!player)
    {
        // Clear missed blinds to prevent infinite loop!
        CRingIterator tmp(table_->getRing());
        if (tmp.start(state|PLAYER_STATE_PLAYING))
	    {
	        do
	        {
	            CPlayer* p = tmp.current()->getPlayer();
	            if (p && p->getSitIn())
        		    p->setMissedBlind(0);
	        }
	        while (tmp.next(state|PLAYER_STATE_PLAYING));
	    }

        printf("doAnte(): no button player\n");
        doCleanup();
        return;
    }

    // Remember small blind in case no-one pays big blind
    // and small blind has to be refunded.
    CPlayer* smallBlindPlayer = NULL;

    char message[100];

    toPay = smBlind;

    CRingIterator it(table_->getDealer());
   
    // Heads Up blinds are handled differently
    if (table_->countPlayers(0) > 2)
    {
        // Start blind queries from player next to button
        ++it;        
    }
    else
    {
        // Heads up => button is small blind
    }

    if (!it.restart(state))
    {
        printf("doAnte: not enough players\n");
        doCleanup();
        return;
    }

    //
    // Get blinds from small blind and big blind,
    // ask for sit-in from the rest
    //

    do {

        if (table_->countPlayers(state|
                                 PLAYER_STATE_PLAYING|
                                 PLAYER_STATE_ALLIN|
                                 PLAYER_STATE_ALLIN_RAISE) <= 1)
        {
            break; // not enough players to start game
        }

        player = it.current()->getPlayer();

        if (buttonRefused[player->getSlot()])
        {
            // This player already refused the button -
            // skip him
            continue;
        }

        Plm_Action action = Action_Timeout;

        // implement check to see if player has enough money to pay "toPay"
        // if he does not, then send him an error message, change his state
        // to PLAYER_STATE_DONE, and send out a actionSitOut(player);
        if (player->getChips() == 0 ||
            (!isTournament && player->getChips() < bigBlind))
        {
            // if he joins again, he must pay 'new player's blind'
            player->setMissedBlind(table_->getLo());
            CpduNotify pdu;
            pdu.sendNotifyMessage(player->getConnfd(),
                                  "You must have enough chips to pay the big blind to join the game.",
                                  NF_NoCloseConnection|NF_StatusMessage);

            action = Action_SitOut;
        }
        else if (toPay != 0)
        {
            // Ask the player for blind
            action = promptBlindPost(player, toPay, smallBlindPlayer == NULL);
        }
        else
        {   // Ask the player for sit-in
            action = promptSitIn(player);
        }

        switch (action)
        {
            case Action_Timeout:
            {   // Timeout on blind request => sitout
                actionSitOut(player);
                sprintf(message, "%s : Sit Out (Timed Out)",
                        player->getUsername());
                table_->setLog(message);

		        // Change player's state to NOTRESPONDING -
		        // ring rebuild will take care of player
                player->stateChange(PLAYER_STATE_NOTRESPONDING);
                
            }
            break;

            case Action_SitIn:
            {
                player->setSitouts(0);

                actionSitIn(player);
                sprintf(message, "%s : Sit In", player->getUsername());
                table_->setLog(message);

                // If player sits in.. update his game history
                table_->gameHistoryUpdate(player);
            }
            break;

            case Action_SitOut:
            {
                // If player has missed blinds, increment sitout counter.
                // Eventually he will be thrown away.
                if (player->getMissedBlind() != 0)
                    player->setSitouts(player->getSitouts() + 1);

                actionSitOut(player);
                sprintf(message, "%s : Sit Out", player->getUsername()); 
                table_->setLog(message);
            }
            break;

            case Action_Ante:
            {
                player->setSitouts(0);

                actionAnte(player, toPay);

                // If player antes.. update his game history
                table_->gameHistoryUpdate(player);

                if (toPay == smBlind)
                {
                    smallBlindPlayer = player;
                    toPay = bigBlind;
                }
                else
                {   // we're done with the blinds - continue
                    // sending sitin queries for the rest
                    toPay = 0;
                }
            }
            break;

            default:
            {
                char buf[128];
                sprintf(buf, "Cplm::doAnte: unknown action %d player %s\n",
                        action, player->getUsername());
                Sys_LogError(buf);
            }
            break;
        }
    }
    while (it.next(state)); 

    if (toPay != 0)
    {
        // Cannot start hand because big blind was not paid.
        // If small blind did pay, refund the money and wrap it up

        if (smallBlindPlayer != NULL)
        {
            smallBlindPlayer->setChips(smallBlindPlayer->getChips() + smBlind);
            CpduPlayer pdu(table_);
            pdu.sendPlayerPdu(smallBlindPlayer, PLAYER_CHIPUPDATE);
        }

        CpduAnnounce pduAnnounce(table_);   
        pduAnnounce.sendAnnounce("Insufficient ante to start game!");

        // To prevent infinite loop when a hand never starts
        // because nobody posts blinds, clear missed blind flags
        // for players sitting in!
        CRingIterator it(table_->getRing());
        if (it.start(state|PLAYER_STATE_PLAYING))
        {
            do
            {
                CPlayer* p = it.current()->getPlayer();
                if (p->getSitIn())
                    p->setMissedBlind(0);
            }
            while (it.next(state|PLAYER_STATE_PLAYING));
        }

        doCleanup();
    }
    else
    {
        // hand starts
        inGame_ = true;
    }

    // The first betting round is different from other rounds
    isFirstRound_ = true;
}


//
// MFUNCTION: Cplm::promptBlindPost
//
// PURPOSE: Prompt the player for blind post
//          and wait for the response.
//
// RETURN: The player's action or Action_Timeout
//         if player did not respond within timeout.
//
Plm_Action Cplm::promptBlindPost(CPlayer* player, const CChips& toAnte, bool isSmallBlind)
{
    bool isTournament =
        CTournament::Inst()->isTournament();

    // Make sure the player has enough chips
    if (!isTournament && (player->getChips() < toAnte))
    {
        char s[100];
        sprintf(s, "ANTE: %s has %9.2f chips ante %9.2f\n",
                player->getUsername(),
                player->getChips().asDouble(), toAnte.asDouble());
        Sys_LogError(s);
        return Action_SitOut;
    }

    // if this player has 'missed blind' 'new player's blind',
    // and he is in between the current blinds, he must wait
    // until next round to join (see house rules: SM would be
    // deprived of the button)


    if (isSmallBlind)
    {
      printf("%s missed blind: %9.2f\n",
             player->getUsername(),
             player->getMissedBlind().asDouble());
    }

    if (isSmallBlind && player->getMissedBlind() != 0)
    {
        // Player has missed blind and he's between SM and BB.
        // Must wait until next hand - house rules.
        CpduNotify pdu;
        pdu.sendNotifyMessage(player->getConnfd(),
                              "You must wait until the next hand to join the game",
                              NF_NoCloseConnection|NF_StatusMessage);
        return Action_SitOut;
    }

    if (isTournament)
    {
        // In tournament everybody antes
        CpduBlindRequest blindRequest(table_);
        blindRequest.sendBlindRequest(player,
                                      toAnte,
                                      replyTime_,
                                      incSequenceNumber());

        player->setMissedBlind(0);

        Sys_Sleep(1000); // Unnecessary pause - could remove
        return Action_Ante;
    }

    Plm_Action rc = Action_Timeout;

    CpduBlindRequest blindRequest(table_);
    blindRequest.sendBlindRequest(player,
                                  toAnte,
                                  replyTime_,
                                  incSequenceNumber());

    Cpdu* pdu = NULL;
    if (player->waitPdu(&pdu, timeout_, getSequenceNumber()) == CPlayer::Pdu_Wait_Timeout)
    {
        player->setMissedBlind(toAnte);
        return Action_Timeout;
    }
    else
    {
        // The pdu must be either blind post or sit out pdu. If not,
        // the client is not obeying the protocol.
        if (!pdu)
        {
            Sys_LogError("Cplm::getBlind: unexpected NULL pdu.");
        }
        else if (pdu->isPdu(PDU_Play_PostBlind))
        {
            player->setMissedBlind(0);
            rc = Action_Ante;
        }
        else if (pdu->isPdu(PDU_Play_RefuseBlind))
        {
            player->setMissedBlind(toAnte);
            rc = Action_SitOut;
        }
        else if (pdu->isPdu(PDU_Play_SitOut))
        {   // deprecated!
            player->setMissedBlind(toAnte);
            rc = Action_SitOut;
        }
        else
        {
            char buf[128];
            sprintf(buf, "Cplm::getBlindPost: unexpected pdu type: %d from player %s.\n",
                    pdu->getType(), player->getUsername());
            Sys_LogError(buf);
        }

        // Pdu has been handled, deallocate
        delete pdu;
        pdu = NULL;
    }

    return rc;
}


Plm_Action Cplm::promptSitIn(CPlayer* player)
{
    Plm_Action rc = Action_Timeout;

    if (CTournament::Inst()->isTournament())
    {
        // Fix tournament double big blind bug
        player->setMissedBlind(0);
        return Action_SitIn;
    }

    if (player->getSitIn())
    {
        rc = Action_SitIn;

        // Player sits in - if he has 'missed blind' or
        // 'new player's blind' he must pay the blind before
        // he's dealt in!
        CChips missedBlind = player->getMissedBlind();
        if (missedBlind > 0)
        {
            Plm_Action action = promptBlindPost(player, missedBlind, FALSE);

            if (action == Action_Ante)
            {   // Must post missed blind before allowed to play
                actionAnte(player, missedBlind, true);
                player->setMissedBlind(0);

                rc = Action_SitIn;
            }
            else if (action == Action_SitOut)
            {
                rc = Action_SitOut;
            }
            else if (action == Action_Timeout)
            {
                rc = Action_Timeout;
            }
        }
        return rc;
    }
    else
    {
        return Action_SitOut;
    }

    return rc;
}


void Cplm::actionAnte(CPlayer* player, const CChips& pay, bool isMissedBlind)
{
    bool isTournament = 
        CTournament::Inst()->isTournament();

    CStrOut message;

    if (!isTournament)
    {
        // This is the old ante logic
        potAnte(player, pay);

        echoAction(player, Action_Ante, pay);
        player->stateChange(PLAYER_STATE_PLAYING);

        if (isMissedBlind)
        {
            message << player->getUsername() 
                    << " : New Player / Missed Blind Ante ("
                    << pay << ')';

        }
        else
        {
            message << player->getUsername()
                    << " : Ante (" 
                    << pay << ')';
        }

        table_->setLog(message.str());
    }
    else
    {   
        // In tournaments we have to handle the case
        // of player not having enough chips for the ante

        player->stateChange(PLAYER_STATE_PLAYING);

        Cpot* pot = table_->getMainPot();
        ASSERT(pot);

        CChips smBlind(DEFAULT_LOW), bigBlind(DEFAULT_HI);
        GetBlinds(table_->getLo(), smBlind, bigBlind);

        static bool smAllIn;

        bool isSmallBlind = (pay != bigBlind);
        if (isSmallBlind)
        {
            smAllIn = false;

            // If the player will go all-in notify clients
            if (player->getChips() < pay)
            {
                smAllIn = true;
                CpduAnteAllIn pdu;
                pdu.sendAllIn(table_, pay);
            }

            table_->setBet(pay);
            pot->setBet(pay);
            actionCall(player, pay, true);
        }
        else
        {
            bool willGoAllIn = (player->getChips() < pay);

            if (player->getChips() > smBlind &&
                !smAllIn)
            {
                // Paying big blind is similar to raise -
                // clear eligible lists first
                for (Cpot* tmpPot = pot; tmpPot != NULL; tmpPot = tmpPot->getNext())
                {
                    tmpPot->killList();
                }
            }

            // Do the 'raise'
            table_->setBet(bigBlind);
            if (pot->getNext())
            {
                // Pot has already been split - last pot
                // gets new bet
                ASSERT(bigBlind > pot->getBet());

                // If the player will go all-in notify clients
                if (willGoAllIn)
                {
                    CpduAnteAllIn pdu;
                    pdu.sendAllIn(table_, bigBlind - pot->getBet());
                }

                pot->getNext()->setBet(bigBlind - pot->getBet());
            }
            else
            {
                // If the player will go all-in notify clients
                if (willGoAllIn)
                {
                    CpduAnteAllIn pdu;
                    pdu.sendAllIn(table_, bigBlind);
                }

                pot->setBet(bigBlind);
            }
            actionCall(player, pay, true);

            if (willGoAllIn)
            {
                // Change state to ALLINRAISE so he is counted
                // 'active' until the next betting round
                player->stateChange(PLAYER_STATE_ALLIN_RAISE);
            }
        }
    }
}


void Cplm::actionSitOut(CPlayer* player)
{
    CpduPlayer::SendPlayerPdu(player, PLAYER_SITOUT);
    player->stateChange(PLAYER_STATE_SITOUT);
}


void Cplm::actionSitIn(CPlayer* player)
{
    CpduPlayer::SendPlayerPdu(player, PLAYER_SITIN);
    player->stateChange(PLAYER_STATE_PLAYING);
}

void Cplm::potAnte(CPlayer* player, const CChips& pay)
{
    ASSERT(player->getChips() >= pay);
    player->setChips(player->getChips() - pay);
    Cpot* pot = table_->getPot();

    player->setCurrentBet(pay);
    pot->addPlayerChips(player, pay);

    if (pay > pot->getStake())
    {
        pot->setStake(pay);
        table_->setBet(pay);
    }

    if (pay > pot->getBet())
    {   // clear eligible list (as if raise)
        pot->killList();
	      pot->setBet(pay);
    }

    if (pay == pot->getStake())
      pot->makeEligible(player);

    player->stateChange(PLAYER_STATE_PLAYING);                

    ASSERT(player->getCurrentBet() <= table_->getBet());

/*
    int i = 0;
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        printf("Pot %d ", i++);
        p->print();
    }
*/
}

// Button player decision.
CPlayer* Cplm::decideButtonPlayer(int* refused)
{
  int loopGuard = 20;

    // Find the first dealer candidate
    CPlayer* dealer = table_->getPlayerFromSlot(table_->getButtonSlot());
    while (!dealer || dealer->matchState(PLAYER_STATE_LOGGINGOUT))
    {
        if (dealer)
            refused[dealer->getSlot()] = 1;

        if (!table_->advanceButton())
            return NULL;
        dealer = table_->getPlayerFromSlot(table_->getButtonSlot());
    }

    CpduButton buttonpdu(table_);
    buttonpdu.sendButton();

    while (!dealer->matchState(PLAYER_STATE_PLAYING))
    {
        Plm_Action action = Action_SitOut;
        
        if (dealer->getMissedBlind() > 0)
        {
            // Player has missed a blind - he can't become the button
            if (!table_->advanceButton())
            {   // couldn't advance the button.. no appropriate players
                if (DEBUG & DEBUG_TABLE)
                {
                    printf("decideButtonPlayer(): Couldn't start this hand.\n");
                }

                return NULL;
            }
            dealer = table_->getPlayerFromSlot(table_->getButtonSlot());
            if (!dealer)
            {// couldn't advance the button.. no appropriate players
                if (DEBUG & DEBUG_TABLE)
                {
                    printf("decideButtonPlayer(): Couldn't start this hand.\n");
                }

                return NULL;
            }

	    if (loopGuard-- <= 0)
	      return NULL;

            continue;
        }
        else
        {
            action = promptSitIn(dealer);
        }


        if (action == Action_SitIn)
        {
            dealer->stateChange(PLAYER_STATE_PLAYING);
            actionSitIn(dealer);

            char message[100];
            sprintf(message, "%s (Dealer) : Sit In", dealer->getUsername());
            table_->setLog(message);            
            buttonpdu.sendButton();
        }
        else
        {
            if (refused[dealer->getSlot()] == 1)
            {   // Gone full round but everybody refused!
                // Sleep for a moment and restart
                Sys_Sleep(2000);
                return NULL;
            }

            refused[dealer->getSlot()] = 1;
            dealer->stateChange(PLAYER_STATE_SITOUT);
            actionSitOut(dealer);

            char message[100];
            sprintf(message, "%s : Sit Out", dealer->getUsername());
            table_->setLog(message);

            if (!table_->advanceButton())
            {   // couldn't advance the button.. no appropriate players
                if (DEBUG & DEBUG_TABLE)
                {
                    printf("decideButtonPlayer(): Couldn't start this hand.\n");
                }

                return NULL;
            }

            dealer = table_->getPlayerFromSlot(table_->getButtonSlot());
            if (!dealer)
            {// couldn't advance the button.. no appropriate players
                if (DEBUG & DEBUG_TABLE)
                {
                    printf("decideButtonPlayer(): Couldn't start this hand.\n");
                }

                return NULL;
            }
        }
    }

    return dealer;
}
