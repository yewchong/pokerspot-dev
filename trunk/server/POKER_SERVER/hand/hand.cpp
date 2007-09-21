/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class Chand
  Created  : 1/1/2001

  OVERVIEW : This class maintains the cards in a poker hand.

 ****************************************************************************/

#include "hand.h"
#include "table/table.h"
#include <vector>
#include <algorithm>
#ifdef _WIN32
using namespace std;
#endif

// Use this define to use the PokerSource evaluator
#define USE_POKEREVAL_LIB_ 1
// Use this define to use our own hand type names
// (PokerSource uses strange names such as "Trip"
// for three of a kind)
#define USE_OWN_HANDTYPE_NAMES_ 1

#ifdef USE_POKEREVAL_LIB_
extern "C" {
  #include "poker_defs.h"
  #include "deck_std.h"
  #include "rules_std.h"
  #include "game_std.h"
  #include "inlines/eval.h"
  #include "inlines/eval_low8.c"
}
#endif

Chand::Chand()
    :
    numcards_(0),
    cards_(NULL),
    handtype_(Hand_Undetermined),
    handval_(0),
    highlight_(0),
    highlight2_(0),
    kicker_(0),
    handvalLo_(LowHandVal_NOTHING)
{}

bool Chand::addCard(CCard *pCard)
{
    CCard *index = cards_;
    CCard *pTmp;

    pTmp = new CCard;
    numcards_++;

    if (pTmp == NULL)
    {
        numcards_--;
        return false;
    }

    pTmp->setCard(pCard->getValue(), pCard->getSuit());

    if (cards_ == NULL)
    {
        cards_ = pTmp;
        return true;
    }

    while (index->getNext() != NULL)
    {
        index = index->getNext();
    }

    index->addLink(pTmp);

    return true;
}

void Chand::free()
{
    CCard *iCard, *jCard;

    iCard = cards_;

    while (iCard != NULL)
    {
        jCard = iCard;
        iCard = iCard->getNext();
        delete jCard;
    };
}


const char* GetHandTypeName(int handtype)
{
    const char* handName = NULL;

    switch (handtype)
    {
    case (Hand_Undetermined):   handName = "Undetermined!"; break;
    case (Hand_HighCard):   handName = "High Card"; break;
    case (Hand_Pair):   handName = "Pair"; break;
    case (Hand_TwoPair):    handName = "Two Pair"; break;
    case (Hand_3ofKind):    handName = "3 of a Kind"; break;
    case (Hand_Straight):   handName = "Straight"; break;
    case (Hand_Flush):  handName = "Flush"; break;
    case (Hand_Fullhouse): handName = "Full House"; break;
    case (Hand_4ofKind):    handName = "4 of a Kind"; break;
    case (Hand_StraightFlush):   handName = "Straight Flush"; break;
    case (Hand_RoyalFlush): handName = "Royal Flush"; break;
    default: handName = "Unknown HandType!"; break;
    }

    return handName;
}


inline int StdHandValToType(int handval)
{
    // This works because the poker-eval hand
    // types are in the same order as ours
    int t = HandVal_HANDTYPE(handval);
    return (t + 1);
}

inline int CardToStdRank(CCard* pCard)
{
    // Our cards go from [2..14], poker-eval
    // cards go from [0..12]
    return pCard->getValue() - (2 - StdDeck_Rank_FIRST);
}

inline int CardToStdSuit(CCard* pCard)
{
    // Our suits go from [1..4], poker-eval
    // cards go from [0..3]
    return pCard->getSuit() - (1 - StdDeck_Suit_FIRST);
}


void Chand::rank(bool hilo)
{
    if (CTable::Instance()->getGameType() == Game_Type_Omaha)
    {
        rankOmaha(hilo);
        return;
    }

    CardMask cards;

    //
    // First build a poker-eval deck out of
    // our cards
    //
    StdDeck_CardMask_RESET(cards);
    CCard* pCard = cards_;
    while (pCard != NULL)
    {
        StdDeck_CardMask_SET(cards, 
                             StdDeck_MAKE_CARD(CardToStdRank(pCard),
                                               CardToStdSuit(pCard)));
        pCard = pCard->getNext();
    }

    //
    // Evaluate the hand
    //
    handval_ = Hand_EVAL_N(cards, numcards_);
    handtype_ = (Hand_Type)StdHandValToType(handval_);

    if (hilo)
    {
      handvalLo_ = Hand_EVAL_LOW8(cards, numcards_);
    }
}

bool Chand::operator >(Chand *opponent)
{
    return (handval_ > opponent->handval_);
}

#ifdef USE_OWN_HANDEVALUATOR_

bool Chand::testHighCard()
{
    int count[13];
    memset(count, 0, sizeof(count));
    int i, j;
    CCard *pCard = cards_;

    if (numcards_ < 2)
        return false;       // not enough cards for a pair


    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 0)
        {
            if (Hand_HighCard > handtype_)
            {
                handtype_ = Hand_HighCard;
                highlight_ = i + 2;         // scientists start at 0
                                            // poker players start at deuce

                for (j = 12; j >= 0; j--)
                {
                    if (j != i)
                        if (count[j])
                        {
                            kicker_ = j + 2;
                            j = -1;
                        }
                }
            }

            return true;
        }
    }

    return false;

}

bool Chand::testPair()
{
    int count[13];
    memset(count, 0, sizeof(count));
    int i, j;
    CCard *pCard = cards_;

    if (numcards_ < 2)
        return false;       // not enough cards for a pair


    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 1)
        {
            if (Hand_Pair > handtype_)
            {
                handtype_ = Hand_Pair;
                highlight_ = i + 2;         // scientists start at 0
                                            // poker players start at deuce

                for (j = 12; j >= 0; j--)
                {
                    if (j != i)
                        if (count[j])
                        {
                            kicker_ = j + 2;
                            j = -1;
                        }
                }
            }

            return true;
        }
    }

    return false;
}

bool Chand::testTwoPair()
{
    int count[13];
    memset(count, 0, sizeof(count));
    CCard *pCard = cards_;
    int i, j;
    int firstPair = 0;

    if (numcards_ < 4)
        return false;       // not enough cards for a two pair

    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 1)
        {
            if (!firstPair)
                firstPair = i;
            else
            {
                if (Hand_TwoPair > handtype_)
                {
                    handtype_ = Hand_TwoPair;
                    highlight_ = firstPair + 2;
                    highlight2_ = i + 2;

                    for (j = 12; j >= 0; j--)
                    {
                        if ((j != i) && (j != firstPair))
                            if (count[j])
                            {
                                kicker_ = j + 2;
                                j = -1;
                            }
                    }
                }

                return true;
            }
        }
    }

    return false;
}

bool Chand::testFullHouse()
{
// First find the trips
    int count[13];
    memset(count, 0, sizeof(count));
    CCard *pCard = cards_;
    int i, j;
    int trips = 0;

    if (numcards_ < 5)
        return false;       // not enough cards for a full house

    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 2)       // first find the trips
        {
            if (!trips)
                trips = i;
            else
            {
                if (Hand_Fullhouse > handtype_)
                {
                    handtype_ = Hand_Fullhouse;
                    highlight_ = trips + 2;
                    highlight2_ = i + 2;

                    for (j = 12; j >= 0; j--)
                    {
                        if ((j != i) && (j != trips))
                            if (count[j])
                            {
                                kicker_ = j + 2;
                                j = -1;
                            }
                    }
                }

                return true;
            }
        }
        else if ((count[i] > 1) && (trips))     // then find the pair
        {
            if (Hand_Fullhouse > handtype_)
            {
                handtype_ = Hand_Fullhouse;
                highlight_ = trips + 2;
                highlight2_ = i + 2;

                for (j = 12; j >= 0; j--)
                {
                    if ((j != i) && (j != trips))
                        if (count[j])
                        {
                            kicker_ = j + 2;
                            j = -1;
                        }
                }
            }

            return true;
        }
    }

    return false;
}

bool Chand::test4ofKind()
{

    int count[13];
    memset(count, 0, sizeof(count));
    int i, j;
    CCard *pCard = cards_;

    if (numcards_ < 4)
        return false;       // not enough cards for a 4 of a kind


    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 3)
        {
            if (Hand_4ofKind > handtype_)
            {
                handtype_ = Hand_4ofKind;
                highlight_ = i + 2;         // scientists start at 0
                                            // poker players start at deuce

                for (j = 12; j >= 0; j--)
                {
                    if (j != i)
                        if (count[j])
                        {
                            kicker_ = j + 2;
                            j = -1;
                        }
                }
            }

            return true;
        }
    }

    return false;
}

bool Chand::test3ofKind()
{
    int count[13];
    memset(count, 0, sizeof(count));
    int i, j;
    CCard *pCard = cards_;

    if (numcards_ < 3)
        return false;       // not enough cards for a 3 of a kind


    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 0; i--)
    {
        if (count[i] > 2)
        {
            if (Hand_3ofKind > handtype_)
            {
                handtype_ = Hand_3ofKind;
                highlight_ = i + 2;         // scientists start at 0
                                            // poker players start at deuce

                for (j = 12; j >= 0; j--)
                {
                    if (j != i)
                        if (count[j])
                        {
                            kicker_ = j + 2;
                            j = -1;
                        }
                }
            }

            return true;
        }
    }

    return false;

}

bool Chand::testStraight()
{
    int count[13];
    memset(count, 0, sizeof(count));
    int i;
    CCard *pCard = cards_;

    if (numcards_ < 5)
        return false;       // not enough cards for a 3 of a kind


    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getValue() - 2]++;
    }

    for (i = 12; i >= 4; i--)
    {
        if ((count[i]) && (count[i-1]) && (count[i-2]) && (count[i-3]) && (count[i-4]))
        {
            if (Hand_Straight > handtype_)
            {
                handtype_ = Hand_Straight;
                highlight_ = i + 2;



            }

            return true;
        }
    }

    // do one more test.. ace-low straight
    if ((count[12]) && (count[0]) && (count[1]) && (count[2]) && (count[3]))
    {
        if (Hand_Straight > handtype_)
        {
            handtype_ = Hand_Straight;
            highlight_ = 5;             // five high (ace-low) straight
        }

        return true;
    }

    return false;

}

bool Chand::testFlush()
{
    int count[4][13];   // suit, then value
    memset(count, 0, sizeof(count));
    int suit[4];
    memset(suit, 0, sizeof(suit));
    int i, j;
    int winval = 0, winsuit = 0;
    CCard *pCard = cards_;

    if (numcards_ < 5)      // not enough cards for a flush
        return false;

    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getSuit() - 1][pCard->getValue() - 2]++;
        suit[pCard->getSuit() - 1]++;
    }

    for (i = 3; i >= 0; i--)
    {
        if (suit[i] > 4)
        {
            for (j = 12; j >= 0; j--)
            {
                if (count[i][j] > winval)
                {
                    winval = count[i][j];
                    winsuit = i;
                }
            }
        }
    }

    if (winval > 0)
    {
        if (Hand_Flush > handtype_)
        {
            handtype_ = Hand_Flush;
            highlight_ = winval;

            for (j = 12; j >= 0; j--)
            {
                if (j != winval)
                    if (count[winsuit][j])
                    {
                        kicker_ = j + 2;
                        j = -1;
                    }
            }
        }

        return true;
    }

    return false;
}

bool Chand::testStraightFlush()
{
    int count[4][13];   // suit, then value
    memset(count, 0, sizeof(count));
    int suit[4];
    memset(suit, 0, sizeof(suit));
    int i, j;
    CCard *pCard = cards_;

    if (numcards_ < 5)      // not enough cards for a flush
        return false;

    for (; pCard != NULL; pCard = pCard->getNext())
    {
        count[pCard->getSuit() - 1][pCard->getValue() - 2]++;
        suit[pCard->getSuit() - 1]++;
    }

    for (i = 3; i >= 0; i--)        // for all suits..
    {
        if (suit[i] > 4)            // test for flush..
        {
            for (j = 12; j >= 4; j--)   // test for straight
            {
                if ((count[i][j]) && (count[i][j-1]) && (count[i][j-2]) &&
                    (count[i][j-3]) && (count[i][j-4]))
                {

                    // is straight flush.
                    if (Hand_StraightFlush > handtype_)
                    {
                        handtype_ = Hand_StraightFlush;
                        highlight_ = j + 2;
                    }

                    return true;
                }
            }

            // do one final test for ace-low strflush..
            if ((count[i][12]) && (count[i][0]) && (count[i][1]) &&
                (count[i][2]) && (count[i][3]))
            {
                if (Hand_StraightFlush > handtype_)
                {
                    handtype_ = Hand_StraightFlush;
                    highlight_ = 5;        // five high (ace-low) strflush
                }

                return true;
            }
        }
    }

    return false;
}

bool Chand::testRoyalFlush()
{
    // A royal flush is just an Ace-high straight flush
    if ((handtype_ == Hand_StraightFlush) && (highlight_ == 14))
    {
        handtype_ = Hand_RoyalFlush;
        return true;
    }

    return false;
}
#endif


Hand_Type Chand::getType()
{
    if (handtype_ == Hand_Undetermined)
        rank();

    return handtype_;
};

static char g_buf[1024];

char *Chand::toString(bool longStr)
{
#if defined(USE_POKEREVAL_LIB_) && !defined(USE_OWN_HANDTYPE_NAMES_)

    if (handval_ != 0)
    {
        if (longStr)
        {   // long format with more information
            StdRules_HandVal_toString(handval_, g_buf);
        }
        else
        {   // hand type only
            sprintf(g_buf, "%s",
                    handTypeNames[HandVal_HANDTYPE(handval_)]);
        }
    }
    else
    {
        sprintf(g_buf, "%s", "Undetermined");
    }

    return g_buf;

#else

    // returns textual representation of hand 

    const char* handName = GetHandTypeName(handtype_);

#if defined(USE_POKEREVAL_LIB_)

    // The PokerSource lib provides additional
    // information about the hand
    if (longStr)
    {
        char* p = g_buf;
        p += sprintf(g_buf, "%s (", handName);
        int htype = HandVal_HANDTYPE(handval_);
        if (StdRules_nSigCards[htype] >= 1)
            p += sprintf(p, "%c", StdDeck_rankChars[HandVal_TOP_CARD(handval_)]);
        if (StdRules_nSigCards[htype] >= 2)
            p += sprintf(p, " %c", StdDeck_rankChars[HandVal_SECOND_CARD(handval_)]);
        if (StdRules_nSigCards[htype] >= 3) 
            p += sprintf(p, " %c", StdDeck_rankChars[HandVal_THIRD_CARD(handval_)]);
        if (StdRules_nSigCards[htype] >= 4)
            p += sprintf(p, " %c", StdDeck_rankChars[HandVal_FOURTH_CARD(handval_)]);
        if (StdRules_nSigCards[htype] >= 5)
            p += sprintf(p, " %c", StdDeck_rankChars[HandVal_FIFTH_CARD(handval_)]);
        p += sprintf(p, ")");
    }
    else
        sprintf(g_buf, "%s", handName);

#else

    sprintf(g_buf, "%s", handName);

#endif

    return g_buf;

#endif
};

// 7-stud
// Indicates true if the third card in this hand is
// lower than the third card in that
bool Chand::compareLowestUpCard(Chand* that)
{
    ASSERT(getNumCards() == 3);
    ASSERT(that->getNumCards() == 3);

    // get third card in both hands

    CCard* thisCard = cards_;
    int i = 0;
    for (i = 0; i < 2; i++)
    {
        thisCard = thisCard->getNext();
    }

    CCard* thatCard = that->cards_;
    for (i = 0; i < 2; i++)
    {
        thatCard = thatCard->getNext();
    }

    if (thisCard->getValue() < thatCard->getValue())
    {
        return true;
    }
    else if (thisCard->getValue() == thatCard->getValue())
    {
        // compare suits, order is (lowest-highest):
        // Clubs, Diamonds, Hearts, Spades
        return thisCard->getSuit() < thatCard->getSuit();
    }

    return false;

}

// 7-stud
// Indicates true if the showing cards of this hand is
// higher than the showing cards of that hand
bool Chand::compareHighestShowingHand(Chand* that)
{
    ASSERT(getNumCards() >= 4);
    ASSERT(that->getNumCards() >= 4);
    ASSERT(getNumCards() == that->getNumCards());

    int numOpenCards = getNumCards() - 2;
    if (getNumCards() == 7)
    {
        // Last card is not open
        numOpenCards--;
    }

    ASSERT(numOpenCards >= 2 && numOpenCards <= 4);

    // get open cards (3,4,5,6) in both hands, compare

    CardMask cards;
    StdDeck_CardMask_RESET(cards);

    //
    // Build a poker-eval deck out of open cards
    //

    // First skip the two pocket cards
    CCard* card = cards_;
    card = card->getNext();
    card = card->getNext();

    int i = numOpenCards;

    while (card != NULL)
    {
        StdDeck_CardMask_SET(cards, 
                             StdDeck_MAKE_CARD(CardToStdRank(card),
                                               CardToStdSuit(card)));
        card = card->getNext();

        if (--i == 0)
            break;
    }

    u_int32_t thisHandval = Hand_EVAL_N(cards, numOpenCards);


    StdDeck_CardMask_RESET(cards);
    card = that->getCard();
    card = card->getNext();
    card = card->getNext();

    i = numOpenCards;

    while (card != NULL)
    {
        StdDeck_CardMask_SET(cards, 
                             StdDeck_MAKE_CARD(CardToStdRank(card),
                                               CardToStdSuit(card)));
        card = card->getNext();

        if (--i == 0)
            break;
    }

    u_int32_t thatHandval = Hand_EVAL_N(cards, numOpenCards);

    if (thisHandval > thatHandval)
    {
        return true;
    }
    else if (thisHandval == thatHandval)
    {
        bool lastOpenCard = getNumCards() == 7;

        // Break tie based on suit of the open cards.
        // The player with highest suit wins, in this order:
        // Spades, Hearts, Diamonds, Clubs.
        u_int16_t highestCard = 0;
        CCard* card = this->getCard();
        // Skip the two pocket cards
        card = card->getNext();
        card = card->getNext();

        // Go through open cards
        while (card && (!lastOpenCard || card->getNext() != NULL))
        {
            u_int16_t thisCard = card->getSuit() * 13 + card->getValue();
            if (thisCard > highestCard)
            {
                highestCard = thisCard;
            }
            card = card->getNext();
        }

        card = that->getCard();
        // Skip the two pocket cards
        card = card->getNext();
        card = card->getNext();
        // Go through open cards
        while (card && (!lastOpenCard || card->getNext() != NULL))
        {
            u_int16_t thisCard = card->getSuit() * 13 + card->getValue();
            if (thisCard > highestCard)
            {
                return false; // that hand is higher
            }
            card = card->getNext();
        }

        return true; // this hand is higher
    }
    else
    {
        return false;
    }

    return false;
}

const char* Chand::getShowingHandString()
{
    int numOpenCards = getNumCards() - 2;
    if (getNumCards() == 7)
    {
        // Last card is not open
        numOpenCards--;
    }

    ASSERT(numOpenCards >= 2 && numOpenCards <= 4);

    // get open cards (3,4,5,6) 

    CardMask cards;
    StdDeck_CardMask_RESET(cards);

    //
    // Build a poker-eval deck out of open cards
    //

    // First skip the two pocket cards
    CCard* card = cards_;
    card = card->getNext();
    card = card->getNext();

    int i = numOpenCards;

    while (card != NULL)
    {
        StdDeck_CardMask_SET(cards, 
                             StdDeck_MAKE_CARD(CardToStdRank(card),
                                               CardToStdSuit(card)));
        card = card->getNext();

        if (--i == 0)
            break;
    }

    u_int32_t hv = Hand_EVAL_N(cards, numOpenCards);
    int ht = StdHandValToType(hv);
    const char* handName = GetHandTypeName(ht);

    sprintf(g_buf, "%s", handName);
    return g_buf;
}

void Chand::rankOmaha(bool hilo)
{
    if (getNumCards() < 9)
    {
        handval_ = 0;
        handtype_ = Hand_Undetermined;
        return;
    }

    // We need to form the best hand consisting of
    // exactly two pocket cards and three board cards.
    int pocket[12] = {0, 1,
                      0, 2,
                      0, 3,
                      1, 2,
                      1, 3,
                      2, 3};
    int board[30] = {0, 1, 2,
                     0, 1, 3,
                     0, 1, 4,
                     0, 2, 3,
                     0, 2, 4,
                     0, 3, 4,
                     1, 2, 3,
                     1, 2, 4,
                     1, 3, 4,
                     2, 3, 4};
    vector<CCard*> pocketCards(4);
    vector<CCard*> boardCards(5);
    
    // Copy the cards to STL vectors we can use for
    // generating the permutations
    CCard* pCard = cards_;
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        pocketCards[i] = pCard;
        pCard = pCard->getNext();
    }

    for (; i < 9; i++)
    {
        boardCards[i - 4] = pCard;
        pCard = pCard->getNext();
    }

    // Generate permutations and find the best hand
    u_int32_t best = 0, lowest = 0;
    CardMask cards;

    int k = 0;
    // For each permutation of pocket cards
    for (i = 0; i < 6; i++)
    {
        int l = 0;
        // For each permutation of board cards
        for (int j = 0; j < 10; j++)
        {
            // Form the poker-eval hand and evaluate
            StdDeck_CardMask_RESET(cards);
            StdDeck_CardMask_SET(cards, 
                                 StdDeck_MAKE_CARD(CardToStdRank(pocketCards[pocket[k]]),
                                                   CardToStdSuit(pocketCards[pocket[k]])));
            StdDeck_CardMask_SET(cards, 
                                 StdDeck_MAKE_CARD(CardToStdRank(pocketCards[pocket[k + 1]]),
                                                   CardToStdSuit(pocketCards[pocket[k + 1]])));
            StdDeck_CardMask_SET(cards, 
                                 StdDeck_MAKE_CARD(CardToStdRank(boardCards[board[l]]),
                                                   CardToStdSuit(boardCards[board[l]])));
            StdDeck_CardMask_SET(cards, 
                                 StdDeck_MAKE_CARD(CardToStdRank(boardCards[board[l + 1]]),
                                                   CardToStdSuit(boardCards[board[l + 1]])));
            StdDeck_CardMask_SET(cards, 
                                 StdDeck_MAKE_CARD(CardToStdRank(boardCards[board[l + 2]]),
                                                   CardToStdSuit(boardCards[board[l + 2]])));

            //
            // Evaluate this hand
            //
            u_int32_t handval = Hand_EVAL_N(cards, 5);
            if (handval > best)
                best = handval;

            if (hilo)
            {
                u_int32_t loval = Hand_EVAL_LOW8(cards, 5);
                if (lowest == 0 || loval < lowest)
                    lowest = loval;
            }

            l += 3;
        }
        k += 2;
    }

    handval_ = best;
    handtype_ = (Hand_Type)StdHandValToType(handval_);

    handvalLo_ = lowest;
}

bool Chand::compareLo(Chand* that)
{
    return handvalLo_ < that->handvalLo_;
}

// This is straight from the poker-eval library:
#define CARD_TO_RANK(c) ((c) == StdDeck_Rank_2 ? StdDeck_Rank_ACE : (c-1))

const char* Chand::lowString(bool longStr) const
{
    // returns textual representation of low hand

    int ht = (Hand_Type)StdHandValToType(handvalLo_);
    const char* handName = GetHandTypeName(ht);

    if (longStr)
    {
        // The PokerSource lib provides additional
        // information about the hand
        char* p = g_buf;
        if (handvalLo_ == LowHandVal_NOTHING)
            p += sprintf(g_buf, "No Low");
        else
        {
            p += sprintf(g_buf, "%s (", handName);
            int htype = HandVal_HANDTYPE(handvalLo_);
            if (StdRules_nSigCards[htype] >= 1)
                p += sprintf(p, "%c", StdDeck_rankChars[CARD_TO_RANK(HandVal_TOP_CARD(handvalLo_))]);
            if (StdRules_nSigCards[htype] >= 2)
                p += sprintf(p, " %c", StdDeck_rankChars[CARD_TO_RANK(HandVal_SECOND_CARD(handvalLo_))]);
            if (StdRules_nSigCards[htype] >= 3) 
                p += sprintf(p, " %c", StdDeck_rankChars[CARD_TO_RANK(HandVal_THIRD_CARD(handvalLo_))]);
            if (StdRules_nSigCards[htype] >= 4)
                p += sprintf(p, " %c", StdDeck_rankChars[CARD_TO_RANK(HandVal_FOURTH_CARD(handvalLo_))]);
            if (StdRules_nSigCards[htype] >= 5)
                p += sprintf(p, " %c", StdDeck_rankChars[CARD_TO_RANK(HandVal_FIFTH_CARD(handvalLo_))]);
            p += sprintf(p, ")");
        }
    }
    else
        sprintf(g_buf, "%s", handName);
    
    return g_buf;
};
