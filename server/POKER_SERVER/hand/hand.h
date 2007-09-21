#ifndef __table_hand_h__
#define __table_hand_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class Chand
  Created  : 1/1/2001

  OVERVIEW : This class maintains the cards in a poker hand.

 ****************************************************************************/


#include "deck/card.h"

enum Hand_Type
{
    Hand_Undetermined   = 0,
    Hand_HighCard       = 1,
    Hand_Pair           = 2,
    Hand_TwoPair        = 3,
    Hand_3ofKind        = 4,
    Hand_Straight       = 5,
    Hand_Flush          = 6,
    Hand_Fullhouse      = 7,
    Hand_4ofKind        = 8,
    Hand_StraightFlush  = 9,
    Hand_RoyalFlush     = 10
};

class Chand
{
public:
    Chand();                    // class constructor
    bool addCard(CCard *);      // Adds a card to this hand
    bool operator>(Chand *);    // Indicates true if this hand is better than that
    void free();                // Free hand
    void rank(bool hilo = false); // Rank hand
    Hand_Type getType();          // returns ranking
    CCard *getCard() const;
    char *toString(bool longStr = true); // returns string representation of HandType

    u_int32_t getHandVal() const;
    int getNumCards() const;

    bool compareLowestUpCard(Chand*);      // Indicates true if this hand is lower than that
    bool compareHighestShowingHand(Chand*);// Indicates true if this hand is higher than that
    const char* getShowingHandString();

    void rankOmaha(bool hilo = false);

    bool compareLo(Chand* that);
    u_int32_t getLowHandVal() const { return handvalLo_; }
    const char* lowString(bool longStr = true) const;

private:
    CCard *cards_;              // pointer to this hand's cards
    int numcards_;              // integer indicating how many cards are in hand
    Hand_Type handtype_;
    u_int32_t handval_;         // PokerSource HandVal
    int highlight_, highlight2_, kicker_;    // important cards of the hand

    u_int32_t handvalLo_;

#ifdef USE_OWN_HANDEVALUATOR_
    bool testHighCard();
    bool testPair();
    bool testTwoPair();
    bool test3ofKind();
    bool testStraight();
    bool testFlush();
    bool testFullHouse();
    bool test4ofKind();
    bool testStraightFlush();
    bool testRoyalFlush();
#endif
};


inline CCard *Chand::getCard() const
{
    return cards_;
};

inline u_int32_t Chand::getHandVal() const
{
    return handval_;
}

inline int Chand::getNumCards() const
{
    return numcards_;
}

#endif // #ifndef __hand_h__
