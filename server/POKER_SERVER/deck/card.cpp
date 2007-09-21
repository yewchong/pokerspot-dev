#include "card.h"

CCard::CCard()
{
	value_ = 0;
	suit_ = 0;
	next_ = 0;
};

CCard::CCard(u_int16_t value, u_int16_t suit)
{
	value_ = value;
	suit_ = suit;
	next_ = 0;
};

bool CCard::operator >(CCard *opponent)
{
    if (value_ > opponent->getValue())
        return true;
    else
        return false;
};

#ifdef DEBUG
void CCard::printCard()
{
	printf("Card [Value/Suit] : ");
	
	switch (value_)
	{
		case (0):	printf("[2/"); break;
		case (1):	printf("[3/"); break;
		case (2):	printf("[4/"); break;
		case (3):	printf("[5/"); break;
		case (4):	printf("[6/"); break;
		case (5):	printf("[7/"); break;
		case (6):	printf("[8/"); break;
		case (7):	printf("[9/"); break;
		case (8):	printf("[10/"); break;
		case (9):	printf("[J/"); break;
		case (10):	printf("[Q/"); break;
		case (11):	printf("[K/"); break;
		case (12):	printf("[A/"); break;
	}

	switch (suit_)
	{
		case (0):	printf("C]\n"); break;
		case (1):	printf("D]\n"); break;
		case (2):	printf("H]\n"); break;
		case (3):	printf("S]\n"); break;
	}

};
#endif

