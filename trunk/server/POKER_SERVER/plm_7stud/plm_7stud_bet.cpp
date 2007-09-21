/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Class    : Cplm_7Stud
  Created  : 06/21/2000

  OVERVIEW : 7 Card Stud logic module. This is a subclass
             of the basic Cplm that implements 7 Card Stud
             specific ante and betting structure.

 ****************************************************************************/

#include "plm_7stud.h"
#include "table/table.h"
#include "ring/ring.h"
#include "pduplay/pduannounce.h"
#include "sys.h"

//
// Bet mfunctions
//
void Cplm_7Stud::doBettingRound(int cap, const CChips& raise)
{
    bettingRound_++;

    // in 1-on-1 tables, there really is no cap -
    // make it 40!
    if (table_->getMaxPlayers() == 2)
        cap = 40;

    CChips ante = 1, bringin = 2;
    if (table_->getLo() < 10)
    {
        ante = 1;
        bringin = 2;
    }
    else
    {
        ante = 2;
        bringin = 5;
    }


    CPlayer* spotlight = NULL;

    if (isFirstRound_)
    {
        // Automatic bring-in by the player who has the
        // lowest upcard
        spotlight = getPlayerWithLowestUpCard();
        if (!spotlight || spotlight->getChips() < bringin)
        {   
            char s[100];
            sprintf(s, "Not enough players for betting round (1)!\n");
            Sys_LogError(s);
            doCleanup();
            return;
        }

        // Bring-in counts as a raise
        cap--;

        actionRaise(spotlight, bringin, bringin, true);
    }
    else
    {   
        // start by the player who has the highest showing hand
        spotlight = getPlayerWithHighestShowingHand();
    }

    CRingIterator it(table_->getRingNode(spotlight));

    if (isFirstRound_)
    {   // first round, action begins from player
        // next to bring in
        it.next(PLAYER_STATE_PLAYING);
    }
   
    if (!it.restart(PLAYER_STATE_PLAYING))
    {
        char s[100];
        sprintf(s, "Not enough players for betting round (2)!\n");
        Sys_LogError(s);
        doCleanup();
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
        if (!table_->getLivePot()->isComplete())
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

        Plm_Action action = promptAction(player, promptPay, toRaise);

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


CPlayer* Cplm_7Stud::getPlayerWithLowestUpCard()
{
    CRingIterator it(table_->getDealer());
    if (!it.restart(PLAYER_STATE_PLAYING))
        return NULL;

    Chand* best = NULL;
    CPlayer* lowest = NULL;

    do
    {
        CPlayer* player = it.current()->getPlayer();
        if (!best)
        {
            best = player->getHand();
            lowest = player;
        }
        else
        {
            Chand* hand = player->getHand();
            if (hand->compareLowestUpCard(best))
            {
                // We have best hand so far 
                best = hand;
                lowest = player;
            }
        }
    }
    while (it.next(PLAYER_STATE_PLAYING));

    return lowest;
}

CPlayer* Cplm_7Stud::getPlayerWithHighestShowingHand()
{
    CRingIterator it(table_->getDealer());
    if (!it.restart(PLAYER_STATE_PLAYING))
        return NULL;

    Chand* best = NULL;
    CPlayer* highest = NULL;

    do
    {
        CPlayer* player = it.current()->getPlayer();
        if (!best)
        {
            best = player->getHand();
            highest = player;
        }
        else
        {
            Chand* hand = player->getHand();
            if (hand->compareHighestShowingHand(best))
            {   // We have best hand so far 
                best = hand;
                highest = player;
            }
        }
    }
    while (it.next(PLAYER_STATE_PLAYING));

    if (highest != NULL)
    {
        CpduAnnounce pduAnnounce(table_);   
        char msg[100];
        sprintf(msg, "%s shows highest hand (%s)",
                highest->getUsername(), best->getShowingHandString());
        pduAnnounce.sendAnnounce(msg, ANNOUNCE_FLAG_MESSAGE);
    }

    return highest;
}

bool Cplm_7Stud::isLastBettingRound()
{
    return bettingRound_ == 5;
}

