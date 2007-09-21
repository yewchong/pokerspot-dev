#include "plm.h"
#include "../ring/ring.h"
#include "../table/table.h"
#include "../player/player.h"
#include "../pduplayer/pduplayer.h"

//
// The cleanup resets the table after a hand.
//
// NOTE: this code is protected with the table MUTEX.
//
void Cplm::doCleanup()
{
    SingleLock l(&table_->getTableMutex());
    l.lock();
  
    highBetter_ = NULL;
    lastToAct_ = NULL; // XXX showdown order
    upCards_ = 0;

    // Before starting new hand rebuild table ring.
    rebuildRing();

    CRingIterator it(table_->getRing());

    // If there are any players in the BOUGHTIN, FOLD, PLAYING state, recognize them, by changing
    // their states to "WAITING".  This will allow them to play next hand.
    if (it.start(PLAYER_STATE_READY))
    {
        do {
            it.current()->getPlayer()->stateChange(PLAYER_STATE_WAITING);
        } while (it.next(PLAYER_STATE_READY));
    }

    // For every player that is PLAYER_STATE_SITTING set currentBet=0
    if (it.start(PLAYER_STATE_SITTING))
    {
        do {
            it.current()->getPlayer()->setCurrentBet(0);
        } while (it.next(PLAYER_STATE_SITTING));
    }

    // Now, get a new deck, and shuffle it.
    table_->newDeck();
    table_->shuffleDeck();

    // Trash the board, and the player's hands
    table_->killHands();

    // Now do potEndGame (this cleans up the pots)
    potEndGame();

    inGame_ = false;    
}
