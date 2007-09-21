#include "deck.h"
#include "table/table.h"
#include "random_generator.h"

#define SPECIFICITY     8


int GetCardnum(int modulo)
{
  double x;
  int expcount;
  unsigned long retval;

  x = CRandomGenerator::Inst()->getRand();

  for (expcount = 0; expcount < SPECIFICITY; expcount++)
  {
    x *= 10;
  }

  retval = x;
  return(retval % modulo);
}

static CRandomGenerator g_RandomGenerator;

CDeck::CDeck()
{
	// Deck Constructor
	firstCard_ = NULL;
	depth_ = 0;    
};

CDeck::~CDeck()
{
    freeDeck();
};

void CDeck::addCard(CCard *addition)
{
	// This function adds a card to the bottom of the Deck
	CCard *tmpCard;

	tmpCard = firstCard_;

	if (tmpCard)
	{
		while (tmpCard->getNext())
		{
			tmpCard = tmpCard->getNext();
		}

		tmpCard->addLink(addition);
	}
	else
	{
		firstCard_ = addition;
	}

	depth_++;
};

CCard *CDeck::popCard(void)
{
    CCard *tmpCard = firstCard_;

    if (tmpCard)
    {
        firstCard_ = firstCard_->getNext();
        return tmpCard;
    }
    else
        return NULL;
};

void CDeck::initDeck(void)
{
	// This function adds all 52 cards to the deck
	CCard *tmpCard;
	u_int16_t value, suit;
    int i;

    if (depth_)
        freeDeck();
    
    for (i = 0; i < DECKSIZE; i++)
	{
		value = (u_int16_t) (depth_ % 13) + 2;
		suit = (u_int16_t) (depth_ % 4) + 1;

		tmpCard = new CCard(value, suit);

		addCard(tmpCard);
	}
};

void CDeck::freeDeck(void)
{
    // This function frees every card in the deck.
    CCard *tmpCard;
    tmpCard = firstCard_;

    while (tmpCard != NULL)
    {
        tmpCard = tmpCard->getNext();
        delete firstCard_;
        firstCard_ = tmpCard;
    }

    firstCard_ = NULL;

    depth_ = 0;
};

// use pseudo-random deck
#undef DEBUG_DECKFILE

void CDeck::shuffleDeck(void)
{
  static bool seeded = false;
  if (!seeded)
    {
      seeded = true;
      int factor = CTable::Instance()->getSeed();
      if (factor == 0)
        factor = 1;

      CRandomGenerator::Inst()->seedRand(factor * time(NULL));
    }

    int t = depth_, i, j = 0, k, redundancy = 10;
    CCard *Xk, *Xj = firstCard_;
    CCard xchg;

#ifdef DEBUG_DECKFILE
    FILE *deckfile = fopen(DEBUG_DECKFILE, "r");
    char val = 0, suit = 0; 
    int vnum = 0, snum = 0;

    while (!feof(deckfile) && (val != 10) && (suit != 10))
    {
        fread(&val, 1, 1, deckfile);
        fread(&suit, 1, 1, deckfile);

        if ((val == 10) || (suit == 10))
            break;

        switch (val)
        {
        case ('T'): vnum = 10; break;
        case ('J'): vnum = 11; break;
        case ('Q'): vnum = 12; break;
        case ('K'): vnum = 13; break;
        case ('A'): vnum = 14; break;
        default: vnum = atoi(&val); break;
        }

        switch (suit)
        {
        case ('C'): snum = 1; break;
        case ('D'): snum = 2; break;
        case ('H'): snum = 3; break;
        case ('S'): snum = 4; break;
        }

        Xj->setCard(vnum, snum);
        Xj = Xj->getNext();
        j++;

    }

    fclose(deckfile);

    for (; redundancy > 0; redundancy--) {
        for (; j < t; j++)
        {

#else

    for (; redundancy > 0; redundancy--) {
        for (j=0; j < t; j++)
        {

#endif
            Xj = firstCard_;

            for (i = 0; i < j; i++)
                Xj = Xj->getNext();

            Xk = Xj;

            k = GetCardnum(t - j);

            for (i = 0; i < k; i++)
                Xk = Xk->getNext();
       
            xchg.setCard(Xk->getValue(), Xk->getSuit());
            Xk->setCard(Xj->getValue(), Xj->getSuit());
            Xj->setCard(xchg.getValue(), xchg.getSuit());

        }
    }
    return;

};

#ifdef DEBUG
void CDeck::printDeck(void)
{
	CCard *tmpCard = firstCard_;
	
	while (tmpCard != 0)
	{
		tmpCard->printCard();
		tmpCard = tmpCard->getNext();
	}
};
#endif


int CDeck::getNumCards() const
{
    int rc = 0;
  	CCard* tmpCard = firstCard_;
	
	while (tmpCard != 0)
	{
        rc++;
		tmpCard = tmpCard->getNext();
	}

    return rc;
}
