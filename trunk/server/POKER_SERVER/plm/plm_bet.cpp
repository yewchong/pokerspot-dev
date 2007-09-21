/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : plm_bet.cpp
  Created  : 1/1/2001

  OVERVIEW : Betting related Cplm mfunctions.

 ****************************************************************************/

#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"
#include "pduplay/pduactionrequest.h"
#include "pduplay/pduactionrequestspreadlimit.h"
#include "pduplay/pduactionspreadlimit.h"
#include "pduplay/pduaction.h"
#include "pduplay/pduactionecho.h"
#include "pduplayer/pduplayer.h"
#include "tournament/tournament.h"
#include "common/stringoutput.h"
#include <algorithm>

//
// Betting related Cplm member functions
//

#ifndef _WIN32
inline u_int16_t min(unsigned int u1, u_int16_t u2)
{
  return (u1 < u2) ? u1 : u2;
}
#endif


void Cplm::doBettingRound(int cap, const CChips& raise)
{
    bool isTournament = CTournament::Inst()->isTournament();

    // in 1-on-1 tables, there really is no cap -
    // make it 40!
    if (table_->getMaxPlayers() == 2)
        cap = 40;

    CRingIterator it(table_->getDealer());

    if (isFirstRound_)
    {
        // Big blind counts as first bet
        cap--;

        int state = PLAYER_STATE_PLAYING;
        if (isTournament)
        {   // In tournaments the blinds
            // can go allin
            state |= (PLAYER_STATE_ALLIN|PLAYER_STATE_ALLIN_RAISE);
        }

        // In Heads Up button goes differently
        bool headsUp = (table_->countPlayers(0) == 2);

        it.next(state); // small blind

        // In heads up pre-flop action starts from small blind
        if (!headsUp)
        {   
            // Start action from big blind
            it.next(state); 
        }

        // will start round from next after big blind
        it.next(state); 
    }
    else
    {   // will start round from next to button
        it.next(PLAYER_STATE_PLAYING);
    }
   
    if (!it.restart(PLAYER_STATE_PLAYING))
    {
        isFirstRound_ = false;
        printf("Not enough players for betting round!\n");
        return;
    }

    do
    {
        CChips toRaise = (cap > 0 ? raise : 0);
        CPlayer* player = it.current()->getPlayer();
        ASSERT(table_->getBet() >= player->getCurrentBet());
        CChips toPay = table_->getBet() - player->getCurrentBet();

        if (player->matchState(PLAYER_STATE_ALLIN_RAISE))
        {   // player raised and went all-in in previous
            // round, only now does he become really inactive
            player->stateChange(PLAYER_STATE_ALLIN);
            continue;
        }

        // Bet completion
        if (!isSpreadLimit() && !table_->getLivePot()->isComplete())
        {
            if (player == highBetter_)
            {   // high better cannot complete bet and cannot raise again
                toRaise = 0;
            }
            else
            {   // bet not completed, cannot do full bet but must complete this
                toRaise = table_->getLivePot()->getBetComplete();
            }
        }

        CChips promptPay = toPay;
        if (toPay > player->getChips())
        {
            promptPay = player->getChips();
            toRaise = 0;
        }
        
        if (promptPay + toRaise > player->getChips())
        {
            toRaise = player->getChips() - promptPay;
        }

        Plm_Action action = Action_Timeout;
        if (isSpreadLimit())
        {
            action = promptActionSpread(player, promptPay, toRaise);
        }
        else
        {
            action = promptAction(player, promptPay, toRaise);
        }

        switch (action)
        {
            case Action_Timeout:
            { 
                // If player logged out in the middle of a hand,
                // he'll fold
                // If player has no chips in pot, he'll fold
                if (player->matchState(PLAYER_STATE_LOGGINGOUT))
                {
                    actionFold(player);
                }
                else 
                {
                    actionTimeout(player, toPay);  // this is the network failure allIn
                }
            }
            break;

            case Action_Fold:
            {
                actionFold(player);
            }
            break;

            case Action_Call:
            {               
                actionCall(player, toPay);
            }
            break;

            case Action_Raise:
            {
                cap--;                
                actionRaise(player, toRaise, raise);

                // When someone raises, the betting will
                // not end until we're back to the same player
                it.restart(it.current());
            }
            break;

            default:
            {
                char buf[128];
                sprintf(buf, "Cplm::doBettingRound: unknown action %d player %s\n",
                        action, player->getUsername());
                Sys_LogError(buf);
            }
            break;
        }

        if (table_->countPlayers(PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN_RAISE) < 2)
            break;
    }
    while (it.next(PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN_RAISE));

    // When betting round is finished, turn players in
    // PLAYER_STATE_ALLIN_RAISE state to PLAYER_STATE_ALLIN
    CRingIterator it2(table_->getDealer());
    if (it2.restart(PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN_RAISE))
    {
        do
        {
            CPlayer* player = it2.current()->getPlayer();
            if (player->matchState(PLAYER_STATE_ALLIN_RAISE))
                player->stateChange(PLAYER_STATE_ALLIN);
        }
        while (it2.next(PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN_RAISE));
    }

    // When betting round is finished, mark all bets completed
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        p->complete();
        p->setBetComplete(0);
    }

    isFirstRound_ = false;
}

//
// MFUNCTION: Cplm::promptAction
//
// PURPOSE: Prompt the player for an action
// and wait for the response.
//
// RETURN: The player's action or Action_Timeout
//         if player did not respond within timeout.
//
Plm_Action Cplm::promptActionSpread(CPlayer* player,
                                    const CChips& toPay,
                                    CChips& raise)
{
    Plm_Action rc = Action_Timeout;
    
    CChips potChips = table_->getMainPot()->getTotalChips();
    CChips maxRaise;

    if (betStructure_ == BS_PotLimit)
    {
        // Maximum raise is the number of chips in pot,
        // or player's available chips whichever is less
        CChips raiseChips = player->getChips();
        if (toPay >= raiseChips)
            raiseChips = 0;
        else
            raiseChips = raiseChips - toPay;
        // Player's call is considered part of the pot
        // for the purposes of pot limit raise
        maxRaise = minimum((potChips + toPay), raiseChips);
    }
    else
    {
        // No limit => maximum raise is whatever
        // the player has available
        CChips raiseChips = player->getChips();
        if (toPay >= raiseChips)
            raiseChips = 0;
        else
            raiseChips = raiseChips - toPay;
        maxRaise = raiseChips;
    }

    CChips raiseLo = raise;
    CChips raiseHi = (raise != 0) ? maxRaise : 0;

    // CHECK against maximum dollar amount (32M)
#ifndef _WIN32
    typedef unsigned long ULONG;
#endif
    if (potChips.asDouble() + toPay.asDouble() + raiseHi.asDouble() > 
        (double)CChips::Max_Dollars)
    {
      Sys_LogError("Maximum dollar amount reached");
      raiseHi = 0;
    }

    raiseHi = minimum(raiseHi, maxRaise);
    raiseLo = minimum(raiseLo, raiseHi);

    CpduActionRequestSpreadLimit actionRequest(table_);
    actionRequest.sendActionRequest(player,
                                    toPay,
                                    raiseLo,
                                    raiseHi,
                                    replyTime_,
                                    incSequenceNumber());

    if (CTournament::Inst()->isTournament() && player->isZombie())
    {   // In tournaments zombies are blinded off

        Sys_Sleep(500); // Could remove this delay - unnecessary.

        // Testing: bots always call for now
        string s = player->getUsername();
        if (s.find("bot_") != string::npos)
        {
          // this is a bot player
          return Action_Call;
        }

        return Action_Fold;
    }

    Cpdu* pdu = NULL;
    if (player->waitPdu(&pdu, timeout_, getSequenceNumber()) == CPlayer::Pdu_Wait_Timeout)
    {
        return Action_Timeout;
    }
    else
    {
        // The pdu must be an action pdu. If not, then
        // the client is not obeying the protocol.
        if (!pdu)
        {
            Sys_LogError("Cplm::getAction: unexpected NULL pdu.");
        }
        else if (pdu->isPdu(PDU_Play_Action))
        {
            CpduAction* actionPdu = static_cast<CpduAction*>(pdu);
            u_int16_t action = actionPdu->getAction();
            rc = (Plm_Action)action;
        }
        else if (pdu->isPdu(PDU_Play_ActionSpreadLimit))
        {
            CpduActionSpreadLimit* actionPdu =
                static_cast<CpduActionSpreadLimit*>(pdu);
            u_int16_t action = actionPdu->getAction();
            raise = actionPdu->getRaise();

            // Client's raise edit box allows the player to raise more than he has!
            // Fix it up here 
            if (raise > raiseHi)
            {
                //char buf[100];
                //sprintf(buf, "Raise ERROR Player %s raise %d max: %d\n",
                //        player->getUsername(), raise, raiseHi);
                //Sys_LogError(buf);
                raise = raiseHi;
            }
            if (raise < raiseLo)
            {
                raise = raiseLo;
            }

            rc = (Plm_Action)action;
        }
        else
        {
            char buf[128];
            sprintf(buf, "Cplm::getAction: unexpected pdu type: %d from player %s.\n",
                    pdu->getType(), player->getUsername());
            Sys_LogError(buf);
        }

        // Pdu has been handled, deallocate
        delete pdu;
        pdu = NULL;
    }

    return rc;
}

//
// MFUNCTION: Cplm::promptAction
//
// PURPOSE: Prompt the player for an action
// and wait for the response.
//
// RETURN: The player's action or Action_Timeout
//         if player did not respond within timeout.
//
Plm_Action Cplm::promptAction(CPlayer* player,
                              const CChips& toPay,
                              const CChips& raise)
{
    Plm_Action rc = Action_Timeout;

    CpduActionRequest actionRequest(table_);
    actionRequest.sendActionRequest(player,
                                    toPay,
                                    raise,
                                    replyTime_,
                                    incSequenceNumber());

    if (CTournament::Inst()->isTournament() && player->isZombie())
    {   // In tournaments zombies are blinded off

        Sys_Sleep(500); // Could remove this delay - unnecessary.

        // Testing: bots always call for now
        string s = player->getUsername();
        if (s.find("bot_") != string::npos)
        {
          // this is a bot player
          return Action_Call;
        }

        return Action_Fold;
    }

    Cpdu* pdu = NULL;
    if (player->waitPdu(&pdu, timeout_, getSequenceNumber()) == CPlayer::Pdu_Wait_Timeout)
    {
        return Action_Timeout;
    }
    else
    {
        // The pdu must be an action pdu. If not, then
        // the client is not obeying the protocol.
        if (!pdu)
        {
            Sys_LogError("Cplm::getAction: unexpected NULL pdu.");
        }
        else if (pdu->isPdu(PDU_Play_Action))
        {
            CpduAction* actionPdu = static_cast<CpduAction*>(pdu);
            u_int16_t action = actionPdu->getAction();
            rc = (Plm_Action)action;
        }
        else
        {
            char buf[128];
            sprintf(buf, "Cplm::getAction: unexpected pdu type: %d from player %s.\n",
                    pdu->getType(), player->getUsername());
            Sys_LogError(buf);
        }

        // Pdu has been handled, deallocate
        delete pdu;
        pdu = NULL;
    }

    return rc;
}

void Cplm::actionTimeout(CPlayer* player, const CChips& pay)
{
    // If this is a tournament, he'll fold.
    // If player has no chips in main pot, he'll fold.
    // Otherwise he goes all in
    Cpot* pot = table_->getMainPot();
    if (pot->getPlayerChips(player) == 0 ||
        CTournament::Inst()->isTournament())
    {
        char message[100];
        sprintf(message, "%s : Timeout", player->getUsername());
        table_->setLog(message);
        actionFold(player);
    }
    else
    {
        // 
        // XXX TODO: If player has too many allins, fold,
        // otherwise do an allin an increment allin counter
        //

        // Timeout => go all in paying 0 chips to the pot
        allIn(player, CChips(0, 0));

        char message[100];
        sprintf(message, "%s : Timeout", player->getUsername());
        table_->setLog(message);
    }

#ifdef DEBUG_POT_
    int i = 0;
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        printf("Pot %d ", i++);
        p->print();
    }
#endif
}


void Cplm::actionFold(CPlayer* player)
{
    player->stateChange(PLAYER_STATE_FOLDED);
    potFold(player);

    // Fold player from all the pots

    char message[100] = { '\0' };

    sprintf(message, "%s : Fold", player->getUsername());
    echoAction(player, Action_Fold, 0);
    table_->setLog(message);
}


void Cplm::actionCall(CPlayer* player, const CChips& pay, bool isAnte)
{
    CStrOut message;

    if (pay >= player->getChips())
    {
        message << player->getUsername() << " : All-In ("
                << player->getChips() << ", " << pay << ')';

        allIn(player, pay);  

        table_->setLog(message.str());
    }
    else
    {   
        // determine whether this is an ante, check or call
        if (isAnte)
            message << player->getUsername() << " : Ante (" << pay << ')';
        else if (player->getCurrentBet() < table_->getBet())
            message << player->getUsername() << " : Call (" << pay << ')';
        else
            message << player->getUsername() << " : Check";

        potCall(player);
        if (isAnte)
            echoAction(player, Action_Ante, pay);
        else
            echoAction(player, Action_Call, pay);
        table_->setLog(message.str());
    }

    if (isLastBettingRound() && lastToAct_ == NULL)
          lastToAct_ = player;
}

void Cplm::actionRaise(CPlayer* player,
                       const CChips& raise,
                       const CChips& realRaise,
                       bool isBringIn)
{
    CStrOut message;

    // determine if this is a bet, raise or bring in
    if (isBringIn)
    {
      message << player->getUsername() << " : Bring In (" << raise << ')';
    }
    else if (player->getCurrentBet() < table_->getBet())
    {
      message << player->getUsername() << " : Raise (" << raise << ')';
    }
    else
    {
      message << player->getUsername() << " : Bet (" << raise << ')';
    }

    potRaise(player, raise);
    echoAction(player, Action_Raise, raise);

    // If player has 0 chips after raise, he "raises and goes all-in".
    // This creates a 0-chip sidepot into which all further raises go
    if (player->getChips() == 0)
    {
        allIn(player, 0);

        // Raise & all-in => set state to PLAYER_STATE_ALLIN_RAISE
        // so he will not pop out from this betting round already
        player->stateChange(PLAYER_STATE_ALLIN_RAISE);
        message << " All-In";
    }
    else
    {   // Normal raise, remember high better
        highBetter_ = player;
    }

    table_->setLog(message.str());

    // If we're on the River, remember last person to
    // act for purposes of showdown
    if (isLastBettingRound())
        lastToAct_ = player;

    // BET COMPLETION
    Cpot* livePot = table_->getLivePot();

    if (livePot->isComplete())
    {
        if (realRaise > raise)
        {
            livePot->unComplete();
            livePot->setBetComplete(realRaise - raise);
        }
    }
    else
    {
        // What if raise is less than betComplete?
        // Can it be?
        if (raise == livePot->getBetComplete())
        {   // bet was completed
            livePot->complete();
            livePot->setBetComplete(0);
        }
    }
}


void Cplm::echoAction(CPlayer* player, Plm_Action action, const CChips& value)
{
    CpduActionEcho pdu(table_);
    pdu.sendActionEcho(player, action, value);
}

void Cplm::potCall(CPlayer *player)
{
    CChips pay = (table_->getBet() - player->getCurrentBet());
    Cpot* pot = table_->getMainPot();
    CChips pay_this_pot;
    int i = 0;

    // this should always hold true..
    if (!(player->getChips() >= pay))
    {
        printf("potCall(): error: player's chips: %9.2f < to_pay: %9.2f\n",
            player->getChips().asDouble(), pay.asDouble());
        return;
    }

    player->setChips(player->getChips() - pay);

#ifdef DEBUG_POT_
        printf("Distribute %s's %9.2f chips among active pots.\n",
            player->getUsername(), pay.asDouble());
#endif

    // the live pot which is last is handled separately
    while (pot->getNext() != NULL)
    {
        if (pot->isActive())
        {
            if (pot->isEligible(player))
            {
                // player has already paid to this pot
                // we've gone a round, deactivate it
                pot->deActivate();
            }
            else
            {
                pot->makeEligible(player);

                // player has to pay the difference between what
                // he has paid so far and the current pot bet
                ASSERT(pot->getBet() >= pot->getPlayerChips(player));
                pay_this_pot = pot->getBet() - pot->getPlayerChips(player);
                pot->addPlayerChips(player, pay_this_pot);
                ASSERT(pay >= pay_this_pot);
                pay -= pay_this_pot;
            }

            if (DEBUG & DEBUG_PLM)
            {
                printf("%s pays %9.2f chips to side pot %d, chips left: %9.2f\n",
                       player->getUsername(),
                       pay_this_pot.asDouble(),
                       i++,
                       pay.asDouble());
            }
        }
        pot = pot->getNext();
    }

    // Now we should have the live pot left

#ifdef DEBUG_POT_
    printf("Live pot gets %9.2f chips.\n", pay.asDouble());
#endif

    // rest of the chips go to the live pot
    pot->addPlayerChips(player, pay);

    if (!pot->isEligible(player))
        pot->makeEligible(player);

    player->setCurrentBet(table_->getBet());

#ifdef DEBUG_POT_
    if (DEBUG & DEBUG_PLM)
    {
        printf("potCall(): %s has %9.2f chips\n",
               player->getUsername(), player->getChips().asDouble());
    }

    i = 0;
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        printf("Pot %d ", i++);
        p->print();
    }
#endif
};

void Cplm::potRaise(CPlayer* player, const CChips& raise)
{
    Cpot* pot = table_->getMainPot();
    
    // go to the last pot ("live pot").
    while (pot->getNext() != NULL)
        pot = pot->getNext();

    // raise the total value of the table bet
    table_->setBet(table_->getBet() + raise);

    if (table_->getMainPot()->isActive())
    {
        if (table_->getMainPot() != table_->getPot())
        { // if this player has already paid to the main pot,
          // it means we've gone a round and the main pot must
          // be deactivated.
            if (table_->getMainPot()->isEligible(player))
                table_->getMainPot()->deActivate();

        }
    }

    // raising means live pot's bet is incremented
    pot->setBet(pot->getBet() + raise);

    // clear the entire eligibility list
    pot->killList();

    // calling will now charge the correct amount
    // of chips from the player
    potCall(player);

    return;
};

void Cplm::potFold(CPlayer* player)
{
    // Fold the player from all the pots
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        p->clearEligible(player);
    }
}

void Cplm::potEndRound(void)
{
    // Marks all but the live pot inactive.
    Cpot *pot = table_->getPot();

    while (pot->getNext() != NULL)
    {
        pot->deActivate();
        pot = pot->getNext();
    }

    // the last pot remains active
};

void Cplm::potEndGame(void)
{
    // set the table's bet to 0
    table_->setBet(0);

    // now tear down all pots.
    table_->killPots();

};


bool Cplm::isLastBettingRound()
{
    return table_->getBoard() && table_->getBoard()->getNumCards() == 5;
}

Cplm::BetStructure Cplm::getBetStructure() const
{
    return betStructure_;
}

bool Cplm::isSpreadLimit() const
{
    return betStructure_ != BS_Limit;
}

void Cplm::setBetStructure(BetStructure bs)
{
    betStructure_ = bs;

    if (betStructure_ != BS_Limit)
    {
        // In spread limit betting players have 10 seconds
        // more time to think
//        int tenSecs = 10 * 1000;
//        replyTime_ = DEFAULT_THINKING_TIME + tenSecs;
//        timeout_  = DEFAULT_THINKING_TIME + DEFAULT_TIMEOUT_TIME + tenSecs;
    }
    else
    {
//        replyTime_ = DEFAULT_THINKING_TIME;
//        timeout_  = DEFAULT_THINKING_TIME + DEFAULT_TIMEOUT_TIME;
    }
}

