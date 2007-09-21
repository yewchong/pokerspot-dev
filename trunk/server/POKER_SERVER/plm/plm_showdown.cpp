
#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"
#include "pduplayer/pduplayer.h"
#include "pduplay/pdushowdown.h"
#include "pduplay/pduannounce.h"
#include "pot/pot.h"
#include "gettime.h"
#include "poker_defs.h"
#include "handval_low.h"
#include <list>
#include <algorithm>
USING_NS_T(std, list);


// Return true if 'player' is in the set of winners 'w'.
inline bool IsWinner(CPlayer* player, Winners& w)
{
  return w.find(CWinner(player)) != w.end();
}

inline bool IsWinner(CPlayer* player,
                     const Winners& w1, bool& hi,
                     const Winners& w2, bool& lo)
{
    CWinner winner(player);
    hi = w1.find(winner) != w1.end();
    lo = w2.find(winner) != w2.end();
    return hi || lo;
}

// Find the winner entry for 'player' from the set of winners 'w'.
inline CWinner* GetWinner(CPlayer* player, Winners& w)
{
  Winners::iterator pos = w.find(CWinner(player));
  if (pos != w.end())
    return const_cast<CWinner*>(&(*pos));
  else
    return NULL;
}

//
// New showdown handles multiple pots
//
void Cplm::doShowdown()
{
    // Lock table mutex because leftover chip distribution
    // goes through raw player list.
    // We also do another round through player to fold out
    // those players who have logged out in the middle of the
    // hand.
    SingleLock l(&table_->getTableMutex());
    l.lock();

    foldLoggedOut();

    Cpot *pPot = table_->getMainPot();
    CPlayer* player = NULL;
    CRingIterator it(table_->getRing()); 
    Chand *hand = NULL;
    CpduShowdown pdu(table_);

    table_->updatePotAverage();
    table_->setLog("*** SUMMARY ***");

    // First thing first, take the rake
    doRake();

    int state = PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN|PLAYER_STATE_ALLIN_RAISE;
    int numPlaying = table_->countPlayers(state);
    if (numPlaying > 1)
    {
        // Now, distribute the board's cards to all player's hands, and rank them
        table_->distributeBoard();

        // Evaluate hands
        if (it.start(state))
        {
            do
            {
                player = it.current()->getPlayer();
                hand = player->getHand();
                hand->rank(hiLo_);
            } 
            while (it.next(state));
        }
    }

    // 
    // Go through pots, decide who wins pot
    // and send announcement
    //
    // main pot 
    Cpot* pot = pPot;
    Winners hiWinners, loWinners;
    announcePot(pot, 1, hiWinners, loWinners);

    // side pots
    int potNum = 2;
    pot = pot->getNext();
    while (pot != NULL)
    {
        if (pot->getChips() > 0)
        {   // network all-ins can cause 0 chip pots
            // if no-one raises after the all-in - skip
            // such pots
            announcePot(pot, potNum++, hiWinners, loWinners);
        }
        pot = pot->getNext();
    }

    if (numPlaying > 1 && hiLo_ && loWinners.size() == 0)
    {
        CpduAnnounce pdu(table_);
        pdu.sendAnnounce("No low hands qualify");
    }

    //
    // Do showdown/showcards - a player's cards are unveiled
    // if he is a winner or if he doesn't have muck cards on.
    //
    if (numPlaying > 1)
    {
        // Do showdown order:
        // Start from last person to act on River or
        // next to dealer
        Cring* ring = NULL;

        if (lastToAct_)
          ring = table_->getRingNode(lastToAct_);
        if (!ring)
          ring = table_->getDealer();
        CRingIterator it(ring);
        if (!lastToAct_)
	      {   // start from next to dealer
            it.next(state);
	      }          
       
        if (it.restart(state))
        {
            u_int32_t bestHandVal = 0;
            do 
            {
                player = it.current()->getPlayer();
                bool bestHandSoFar = (player->hasHand() && player->getHand()->getHandVal() >= bestHandVal);
                bool isHiWinner = false, isLoWinner = false;

                if (IsWinner(player,             // cards shown if wins pot
                             hiWinners, isHiWinner,
                             loWinners, isLoWinner) || 
                    player == lastToAct_ ||      // last one to bet shows cards
                    bestHandSoFar ||             // cards shown if beat hands shown so far
                    (!player->getMuckCards()))   // cards shown if loser and muck cards off
                {   
                    // Unveil player's cards

                    if (bestHandSoFar)
                        bestHandVal = player->getHand()->getHandVal();

                    if (table_->getGameType() == Game_Type_Holdem)
                    {
                        if (isLoWinner && !isHiWinner)
                            pdu.sendShowdown(player, 2, player->getHand()->lowString(false));
                        else
                            pdu.sendShowdown(player, 2, player->getHand()->toString(false));
                    }
                    else if (table_->getGameType() == Game_Type_Omaha)
                    {   
                        if (isLoWinner && !isHiWinner)
                            pdu.sendShowdown(player, 4, player->getHand()->lowString(false));
                        else
                            pdu.sendShowdown(player, 4, player->getHand()->toString(false));
                    }
                    else
                    {   
                        int numCards = player->getHand()->getNumCards();
                        int boardCards = 0;
                        if (table_->getBoard())
                            boardCards = table_->getBoard()->getNumCards();
                        if (isLoWinner && !isHiWinner)
                        {
                            pdu.sendShowdown(player,
                                             player->getHand()->getNumCards() - boardCards,
                                             player->getHand()->lowString(false));
                        }
                        else
                        {
                            pdu.sendShowdown(player,
                                             player->getHand()->getNumCards() - boardCards,
                                             player->getHand()->toString(false));
                        }
                    }
                }
                else
                {   // Muck player's cards
                    pdu.sendShowdown(player, 0, "Muck");
                } 
            }
            while (it.next(state));
        }
    }

    char s[200];
    sprintf(s, "Game ends at %s", GetTime());
    table_->setLog(s);

    table_->flushLog();

    // After showdown sleep for a couple of seconds
    // to allow the players to see the hands
    if (table_->getGameType() == Game_Type_Holdem)
      Sys_Sleep(4000);
    else
      Sys_Sleep(5000);
}

//
// Evaluate & announce the winner of 'pot',
// return ptr to the winner.
//
// NOTE: If there are multiple winners and the division
// of chips is not even, there will be chips left over.
// The left over chips will be given to the player closest
// to the left of the dealer.
//
int Cplm::announcePot(Cpot* pot, int potNum,
                      Winners& hiW, Winners& loW)
{
    Chand* hiBest = NULL, * loBest = NULL;

    // Collect winners of this pot to this container 
    Winners hiWinners, loWinners;

    //
    // Compare eligible player's hands, decide winner(s)
    //
    int numEligible = 0;
    for (int i = 0; i < 10; i++)
    {
        CPlayer* player = pot->getEligible(i);
        if (player)
        {
            numEligible++;

            // Decide high winner
            if (!hiBest)
            {
                hiBest = player->getHand();
                hiWinners.insert(player);
            }
            else
            {
                Chand* hand = player->getHand();
                if (*hand > hiBest)
                {   // We have best hi hand so far 
                    hiBest = hand;
                    hiWinners.clear();
                    hiWinners.insert(player);
                }
                else if (hand->getHandVal() == hiBest->getHandVal())
                {
                    // Hands are equal - add to winners
                    hiWinners.insert(player);
                }
            }

            if (hiLo_)
            {
                // Decide low winner
                if (!loBest)
                {
                    Chand* hand = player->getHand();
                    if (hand && hand->getLowHandVal() != LowHandVal_NOTHING)
                    {
                      loBest = hand;
                      loWinners.insert(player);
                    }
                }
                else
                {
                    Chand* hand = player->getHand();
                    if (hand->compareLo(loBest))
                    {   // We have best lo hand so far 
                        loBest = hand;
                        loWinners.clear();
                        loWinners.insert(player);
                    }
                    else if (hand->getLowHandVal() == loBest->getLowHandVal())
                    {
                        // Hands are equal - add to winners
                        loWinners.insert(player);
                    }
                }
            }

        }
    }

    //
    // Compute number of chips given to each winner
    //
    CChips hiChips = pot->getChips(),
           loChips;

    if (hiLo_ && loWinners.size() > 0)
    {
        // High hand winner takes odd chip
        loChips = hiChips / 2;
        hiChips = hiChips - loChips;
    }

    distributeChips(hiChips, hiWinners);

    if (hiLo_ && loWinners.size() > 0)
        distributeChips(loChips, loWinners);

    //
    // Announce high winner(s), award chips
    //
    Winners::iterator it, end;
    for (it = hiWinners.begin(), end = hiWinners.end();
         it != end;
         ++it)
    {
        CWinner& winnerEntry = const_cast<CWinner&>(*it);
        CPlayer* winner = winnerEntry.player_;

        if (winner)
        {
            char msg[300];
            // Show hand type only if more than 1 eligible player
	          // (fix bug "undetermined hand" in hand logs)
            if (numEligible > 1 && winner->hasHand())	    
            {
                if (hiLo_)
                {
                    sprintf(msg, "%s wins %9.2f chips (pot %d) with %s (hi)!",
                            winner->getUsername(),
                            winnerEntry.chips_.asDouble(), potNum,
                            winner->getHand()->toString());
                }
                else
                {
                    sprintf(msg, "%s wins %9.2f chips (pot %d) with %s!",
                            winner->getUsername(),
                            winnerEntry.chips_.asDouble(), potNum,
                            winner->getHand()->toString());
                }
            }
            else
            {
                if (hiLo_)
                {
                    sprintf(msg, "%s wins %9.2f chips (pot %d) (hi)!",
                            winner->getUsername(),
                            winnerEntry.chips_.asDouble(), potNum);
                }
                else
                {
                    sprintf(msg, "%s wins %9.2f chips (pot %d)!",
                            winner->getUsername(),
                            winnerEntry.chips_.asDouble(), potNum);
                }
            }

            table_->setLog(msg);

            CChips chipsWon = winnerEntry.chips_;

            CpduAnnounce pduAnnounce(table_);
    
            if (numEligible > 1)
            {
                // Send the announcement with hand name
                pduAnnounce.sendAnnounce(winner, chipsWon,
                                         winner->getHand()->toString(),
                                         hiLo_ ? "hi" : NULL);
            }
            else
            {
                // Send last man standing announcement
                pduAnnounce.sendAnnounce(winner, chipsWon);
            }

            // Now award the winner and reset state if player was ALLIN
            if (winner->matchState(PLAYER_STATE_ALLIN|PLAYER_STATE_ALLIN_RAISE) && winner->getChips() == 0)
                winner->stateChange(PLAYER_STATE_PLAYING); // player was all-in - reset state
            winner->setChips(winner->getChips() + chipsWon);
    
            // And now send a ChipUpdate Player PDU
            CpduPlayer pduPlayer(table_);
            pduPlayer.sendPlayerPdu(winner, PLAYER_CHIPUPDATE);

            // Winners are returned to caller
            hiW.insert(winner);
        }
    }

    //
    // Announce low winner(s), award chips
    //
    for (it = loWinners.begin(), end = loWinners.end();
         it != end;
         ++it)
    {
        CWinner& winnerEntry = const_cast<CWinner&>(*it);
        CPlayer* winner = winnerEntry.player_;

        if (winner)
        {
            char msg[300];
            if (numEligible > 1 && winner->hasHand())
            {
                sprintf(msg, "%s wins %9.2f chips (pot %d) with %s (low)!",
                        winner->getUsername(),
                        winnerEntry.chips_.asDouble(), potNum,
                        winner->getHand()->lowString());
            }
            else
            {
                sprintf(msg, "%s wins %9.2f chips (pot %d) (hi)!",
                        winner->getUsername(),
                        winnerEntry.chips_.asDouble(), potNum);
            }

            table_->setLog(msg);

            CChips chipsWon = winnerEntry.chips_;

            CpduAnnounce pduAnnounce(table_);
    
            if (numEligible > 1)
            {
                // Send the announcement with hand name
                pduAnnounce.sendAnnounce(winner, chipsWon,
                                         winner->getHand()->lowString(),
                                         "low");
            }
            else
            {
                // Send last man standing announcement
                pduAnnounce.sendAnnounce(winner, chipsWon);
            }

            // Now award the winner and reset state if player was ALLIN
            if (winner->matchState(PLAYER_STATE_ALLIN|PLAYER_STATE_ALLIN_RAISE) && winner->getChips() == 0)
                winner->stateChange(PLAYER_STATE_PLAYING); // player was all-in - reset state
            winner->setChips(winner->getChips() + chipsWon);
    
            // And now send a ChipUpdate Player PDU
            CpduPlayer pduPlayer(table_);
            pduPlayer.sendPlayerPdu(winner, PLAYER_CHIPUPDATE);

            // Winners are returned to caller
            loW.insert(winner);
        }
    }


    return hiWinners.size() + loWinners.size();
}


CPlayer* Cplm::getPlayerClosestToLeftOfDealer(Winners& w)
{
    CRingIterator it(table_->getDealer());
    ++it;
    it.start(0);

    do
    {
        if (it.current()->getPlayer())
        {
            if (IsWinner(it.current()->getPlayer(), w))
            {
                return it.current()->getPlayer();
            }
        }
    }       
    while (it.next(0));

    return NULL;
}

void Cplm::distributeChips(const CChips& potChips, Winners& winners)
{
  if (winners.size() == 0)
  {
    if (potChips != 0)
    {
      char s[200];
      sprintf(s, "No winner for pot, chips: %9.2f\n", potChips.asDouble());
      Sys_LogError(s);
    }
    return;
  }
  
  if (winners.size() == 1)
  {   // simple case: all chips go to the
    // single winner
    const_cast<CWinner&>((*winners.begin())).chips_ = potChips;
    return;
  }

  // Everybody gets (chips in pot / number of winners)
  // Left-over chips are distributed evenly
  CChips chipsLeftOver;
  int numWinners = winners.size();
  if (numWinners > 1)
    chipsLeftOver = potChips - ((potChips / numWinners) * numWinners);
  
  // first give (chips in pot / number of winners)
  // to everybody
  CChips chipsForAll = (potChips / numWinners);
  
  for (Winners::iterator i = winners.begin(),
                         e = winners.end();
       i != e;
       ++i)
  {
    (const_cast<CWinner&>(*i)).chips_ = chipsForAll;
  }
  
  //
  // Distribute left over chips.
  //
  // This is complicated because the division might not be even.
  // Fore instance, assume we have 9 chips and 5 winners.
  // In this case the everybody should get 2 chips except
  // the dealer will get 1 chip.
  //
  // This is done by giving the chips one by one
  // to winners, beginning from player closest to
  // left of dealer, until we're out of chips
  //
  if (chipsLeftOver != 0)
  {
    CPlayer* closestToLeftOfDealer = getOddChipWinner(winners);

    if (closestToLeftOfDealer)
    {
      int slot = closestToLeftOfDealer->getSlot();
      
      while (chipsLeftOver > 0)
      {
        CPlayer* player = table_->getPlayerRawSlot(slot);
        if (player)
        {
          CWinner* winnerEntry = GetWinner(player, winners);
          if (winnerEntry)
          {
            winnerEntry->chips_ += 1;
            chipsLeftOver -= 1;
          }
        }
        slot++;
        if (slot >= 10)
          slot = 0;
      }
    }
  }
}


void Cplm::foldLoggedOut()
{
  for (int i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table_->getPlayerRawSlot(i);
    if (player->matchState(PLAYER_STATE_LOGGINGOUT))
    {      
      // Fold the player from all the pots
      for (Cpot* p = table_->getMainPot(); p != NULL; p = p->getNext())
        p->clearEligible(player);
    }
  }
}


CPlayer* Cplm::getOddChipWinner(Winners& w)
{
  return getPlayerClosestToLeftOfDealer(w);
}
