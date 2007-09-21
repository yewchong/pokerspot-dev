#ifndef __card_h__
#define __card_h__

#include "../includes.h"

class CCard
{
	public:
		CCard();
		CCard(u_int16_t, u_int16_t);
#ifdef DEBUG
		void printCard();
#endif
		void setCard(u_int16_t value = 0, u_int16_t suit = 0);
		void addLink(CCard *);
		CCard *getNext() const;
        u_int16_t getValue() const;
        u_int16_t getSuit() const;
        bool operator >(CCard *);

	private:
		u_int16_t value_;
		u_int16_t suit_;
		CCard *next_;
};


inline void CCard::setCard(u_int16_t value, u_int16_t suit)
{
	value_ = value;
	suit_ = suit;
};

inline void CCard::addLink(CCard *next)
{
	next_ = next;
};

inline CCard *CCard::getNext() const
{
	return next_;
};

inline u_int16_t CCard::getValue() const
{
    return value_;
};

inline u_int16_t CCard::getSuit() const
{
    return suit_;
};

#endif