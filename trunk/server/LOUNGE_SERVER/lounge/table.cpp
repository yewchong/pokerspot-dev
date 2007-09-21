/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTable
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             table server.

 ****************************************************************************/

#include "lounge/table.h"
#include "nocasecompare.h"
#include "pdu/pdu.h"
#include <stdio.h>

using namespace std;

CTable::CTable(SOCKET sd, u_long addr)
    :
    sd_        (sd),
    gametype_  (0),
    number_    (0),
    lo_        (0),
    hi_        (0),
    numPlayers_(0),
    addr_      (addr),
    port_      (0),
    state_     (TS_Idle)
{}

CTable::CTable(const CTable& rhs)
    :
    sd_         (rhs.sd_),
    gametype_   (rhs.gametype_),
    number_     (rhs.number_),
    lo_         (rhs.lo_),
    hi_         (rhs.hi_),
    numPlayers_ (rhs.numPlayers_),
    addr_       (rhs.addr_),
    port_       (rhs.port_),
    state_      (rhs.state_)
{}

CTable& CTable::operator = (const CTable& rhs)
{
    if (this != &rhs)
    {
        sd_ = rhs.sd_;
        gametype_ = rhs.gametype_;
        number_ = rhs.number_;
        lo_ = rhs.lo_;
        hi_ = rhs.hi_;
        numPlayers_ = rhs.numPlayers_;
        addr_ = rhs.addr_;
        port_ = rhs.port_;
        players_ = rhs.players_;
        state_ = rhs.state_;
    }
    return *this;
}

string CTable::getTitle() const
{
    char buf[64];
    sprintf(buf, "Table %d", number_);
    return buf;
}

void CTable::playerSeated(const char* username)
{
    printf("CTable::playerSeated: %s\n", username);

    string newuser(username);

    for (list<string>::iterator pb = players_.begin(),
                                pe = players_.end();
         pb != pe;
         ++pb)
    {
        if (nocasecompare(newuser, (*pb)))
            return; // alr there, don't add twice
    }

    players_.push_back(newuser);
}

void CTable::playerUnseated(const char* username)
{
    printf("CTable::playerUnseated: %s\n", username);

    bool found = false;
    string user(username);

    for (list<string>::iterator b = players_.begin(),
                                e = players_.end();
         b != e;
         ++b)
    {
        if (nocasecompare(user, (*b)))
        {
            players_.erase(b);
            found = true;
            break;
        }
    }

    if (!found)
    {
        printf("*** CTable::playerUnseated: %s NOT FOUND\n", username);
    }
}


bool CTable::isPlayerSeated(const char* username)
{
    string s(username);

    for (list<string>::iterator b = players_.begin(),
                                e = players_.end();
         b != e;
         ++b)
    {
        if (nocasecompare(s, (*b)))
        {
            return true;
        }
    }

    return false;
}


int CTable::send(Cpdu& pdu)
{
  return pdu.sendTo(getSocket());
}
