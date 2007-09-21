
#ifndef __deck_h__
#define __deck_h__

#ifndef __card_h__
#include "card.h"
#endif

#include "../includes.h"
#include "../player/player.h"
#include "../pduplay/pdudealcardsflex.h"

// This is the size of the deck
#define DECKSIZE	52
#define DEBUG_DECKFILE  "deck.dat"



class CDeck
{
	public:
		CDeck();
        ~CDeck();
		void addCard(CCard *);
        CCard *popCard(void);
		void initDeck(void);
		void shuffleDeck(void);

    // XXX 7-stud
    int getNumCards() const;

#ifdef DEBUG
		void printDeck(void);
#endif
		void freeDeck(void);
	private:
		int depth_;
		CCard *firstCard_;
};

#endif