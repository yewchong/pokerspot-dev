/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : plm_allin.cpp
  Created  : 1/1/2001

  OVERVIEW : Allin related Cplm mfunctions.

 ****************************************************************************/

#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"
#include "pduplay/pduallin.h"

void Cplm::allIn(CPlayer* player, const CChips& pay)
{
    /*
     * 1) set player->state(ALLIN)
     * 2) create a new side pot
     * 3) move excess chips from the original pot to the side pot, and
     * 4) make players eligible for active pot (besides all-in player) eligible
     *    for side pot
     * 5) send allin pdu
     */
    CpduAllIn pdu(table_);

    player->stateChange(PLAYER_STATE_ALLIN);

    splitPot(player, pay);

    pdu.sendAllIn(player, pay);

#ifdef DEBUG_POT_
    int i = 0;
    for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
    {
        printf("Pot %d ", i++);
        p->print();
    }
#endif
}

void Cplm::insertPot()
{
    Cpot* oldPot = NULL,
        * newPot = NULL;
    int i = 0;

    newPot = table_->getMainPot();

    // first, find the last active pot at the table
    while (newPot->getNext() != NULL)
    {
        newPot = newPot->getNext();
    }

    if (!newPot->isActive())
    {
        if (DEBUG & DEBUG_PLM)
        {
            printf("insertPot(): The last pot wasn't active!  This shouldn't happen!\n");
            return;
        }
    }

    newPot->setNext(new Cpot);

    if (newPot->getNext() == NULL)
    {
        if (DEBUG & DEBUG_PLM)
        {
            printf("insertPot(): Not enough memory to add link to new pot!\n");
            return;
        }
    }

    oldPot = newPot;
    newPot = newPot->getNext();

    for (i = 0; i < 10; i++)
    {
        if (oldPot->isEligible(table_->getPlayerFromSlot(i)))
        {
            newPot->makeEligible(table_->getPlayerFromSlot(i));
        }
    }
};

//
// Final all-in implementation 05/18/2000
// The code is essentially identical to what we do
// in ordinary call: go through active pots and always
// pay pot's bet. However, since we're going all in,
// in one of the pots the player won't be able to match
// the bet. 
//
// - The first active pot for which the player cannot
//   match the bet is split
// - Excess chips are moved to the new sidepot
// - This routine might not end up splitting any
//   pot if player's chips and pot bets match so
//   that he can match pot N bet and has 0 chips
//   left for pot N+1.
// 
// Example 1, A has infinite chips, he has bet 5,
// B has 3 chips, C has 2 chips. Initial situation:
//
//  PLAYER:     A   B   C
//  POT1:       5   5   5
//
//  A bets 5:
//  PLAYER:     A   B   C
//  POT1:       10  5   5
//   
//  B goes all-in with 3 chips:
//  PLAYER:     A   B   C
//  POT:        8   8   5
//  POT:        2
//
//  C goes all-in with 2 chips:
//  PLAYER:     A   B   C
//  POT:        7   7   7
//  POT:        1   1   
//  POT:        2
//
//  Hence the rule: first active pot for which player
//  cannot match the bet is split.
//
//

void Cplm::splitPot(CPlayer* player, const CChips& pay)
{
    int i = 0;
    bool wasSplit = false;

    // If player has 0 chips left need to split pot
    bool splitInAnyCase = (player->getChips() == 0);

    // Take the chips from the player now - 
    // if this is network allin, 'pay' will be
    // smaller - otherwise player's chips should
    // be smaller
    CChips chips = minimum(player->getChips(), pay);
    player->setChips(player->getChips() - chips);

    Cpot* pot = table_->getMainPot();

    // find the pot which will be split (if any),
    // and distribute chips as we go
    while (pot->getNext() != NULL)
    {
        if (pot->isActive())
        {
            // player has to pay the difference between what
            // he has paid so far and the current pot bet
            ASSERT(pot->getBet() >= pot->getPlayerChips(player));
            CChips pay_this_pot = pot->getBet() - pot->getPlayerChips(player);

            if (chips >= pay_this_pot)
            {   //
                // player can match this pot's bet - pay it,
                // make player eligible, and continue to next pot
                //
                pot->addPlayerChips(player, pay_this_pot);
                chips -= pay_this_pot;
                pot->makeEligible(player);
            }
            else
            {
                //
                // player cannot match this pot's bet -
                // if player has chips left, split the pot,
                // and we're done
                // 
                if (splitInAnyCase || chips > 0)
                {
                    // okay split this pot
                    splitPot(pot, player, chips, pay_this_pot);
                    pot->makeEligible(player);
                    chips = 0;
                    wasSplit = true;
                }
                else
                {
                    // player's chips matched evenly, no need to split
                    // any pot
                    wasSplit = true;
                }

                // we're done, exit loop
                break;
            }


            if (DEBUG & DEBUG_PLM)
            {
                printf("%s pays %d chips to side pot %d, chips left: %d\n",
                    player->getUsername(), pay_this_pot, i++, pay);
            }
        }
        pot = pot->getNext();
    }

    if (!wasSplit)
    {
        // Split the last pot 
        ASSERT(pot->isActive()); // last pot must be active
        ASSERT(pot->getBet() >= pot->getPlayerChips(player));
        CChips pay_this_pot = pot->getBet() - pot->getPlayerChips(player);
        splitPot(pot, player, chips, pay_this_pot);
        pot->makeEligible(player);
    }
}

//
// This mfunction
// - creates new side pot after 'oldPot'
// - makes player eligible to 'oldPot' eligible
// - to new side pot
// - pays the player's chips to 'oldPot' and
//   moves excess chips to new side pot
//
Cpot* Cplm::splitPot(Cpot* oldPot, CPlayer* player,
                     const CChips& player_chips,
                     const CChips& pay_this_pot)
{
    // Create new pot and insert it after 'oldPot' -
    // standard linked list insert
    Cpot* newPot = new Cpot();
    Cpot* nextPot = oldPot->getNext();
    oldPot->setNext(newPot);
    newPot->setNext(nextPot);

    // Make players eligible to oldPot eligible to newly
    // splitted pot
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        if (oldPot->isEligible(table_->getPlayerFromSlot(i)))
            newPot->makeEligible(table_->getPlayerFromSlot(i));
    }

    // Move excess chips from old pot to newly splitted pot

    // How many chips is the player short of?
    ASSERT(pay_this_pot >= player_chips);
    CChips missing_chips = pay_this_pot - player_chips;

    // oldPot gets all of players chips
    oldPot->addPlayerChips(player, player_chips);

    // side pot bet becomes what the allin player
    // was not able to pay (ie the excess chips)
    newPot->setBet(missing_chips);

    // old live pot's bet becomes what the
    // all-in player did pay
    CChips allin_chips_in_pot = oldPot->getPlayerChips(player);
    oldPot->setBet(allin_chips_in_pot);

    // move excess chips from old pot
    // to new side pot
    for (i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* p = table_->getPlayerFromSlot(i);
        if (p)
        {
            if (oldPot->getPlayerChips(p) > allin_chips_in_pot)
            {   // this player has more in pot than the allin player was able to
                // afford - move excess chips to side pto
                CChips excess_chips = (oldPot->getPlayerChips(p) - allin_chips_in_pot);
                newPot->addPlayerChips(p, excess_chips);
                oldPot->subtractPlayerChips(p, excess_chips);
            }
        }
    }

    return newPot;
}
