#ifndef __psi_loungedata_h__
#define __psi_loungedata_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : loungedata.h
  Created  : 02/06/2000

  OVERVIEW : Data structure definitions for maintaining the
             lounge data structures.

             The lounge data is essentially a list (games) of
             lists (queues) of lists (players).

 ****************************************************************************/

#include <list>
#include <string>
#include "network/network.h"

BEGIN_NAMESPACE(Lounge)

USING_NS_TYPE(std, list);
USING_NS_TYPE(std, string);

//--------------------- 
// A player in a queue
//
struct Player
{
  Player() {}
  Player(const Player& p)
    : name_(p.name) {}
  Player(const string& name)
    : name_(name) {}
  Player& operator = (const Player& rhs)
  {
    if (&rhs != this)
      name_ = rhs.name_;
    return *this;
  }
  string name_;
};

inline
bool operator == (const Player& lhs, const Player& rhs)
{
  return lhs.name_ == rhs.name_;
}

typedef list<Player> Players;

//---------------------
// One queue in a game
//
struct Queue
{
  Queue()
    : lomimit_(0), hilimit_(0) {}
  Queue(const Queue& q)
    : lolimit_(q.lolimit_),
      hilimit_(q.hilimit_),
      players_(q.players_) {}
  Queue(u_int16_t lo, u_int16_t hi)
    : lolimit_(lo), hilimit_(hi) {}
  Queue& operator = (const Queue& rhs)
  {
    if (this != &rhs)
    {
      lolimit_ = rhs.lolimit_;
      hilimit_ = rhs.hilimit_;
      players_ = rhs.players_;
    }
    return *this;
  }

  string getTitle() const
  {
    string s;
    s << "$" << lolimit_ << "-$" << hilimit_;
    return s;
  }

  u_int16_t lolimit_;
  u_int16_t hilimit_;  
  Players players_;
};


inline
bool operator == (const Queue& lhs, const Queue& rhs)
{
  return lhs.lolimit_ == rhs.lolimit_ &&
         lhs.hilimit_ == rhs.hilimit_;
}

typedef list<Queue> Queues;

//-----------------------
// A game maintains list
// of queues.
struct Game
{
  Game() {}
  Game(const Game& g)
    : name_(g.name_),
      queues_(g.queues_) {}
  Game(const string& name)
    : name_(name) {}
  Game& operator = (const Game& rhs)
  {
    if (this != &rhs)
    {
      name_ = rhs.name_;
      queues_ = rhs.queues_;
    }
    return *this;
  }
  
  string name_;
  Queues queues;
};

inline
bool operator == (const Game& lhs, const Game& rhs)
{
  return lhs.name_ == rhs.name_;
}

typedef list<Game> Games;

//------------
// Operations
// 

void AddGame     (const string& name);
void AddQueue    (const string& game,
                  u_int16_t lolimit,
                  u_int16_t hilimit);
void AddPlayer   (const string& game,
                  u_int16_t lolimit,
                  u_int16_t hilimit,
                  const string& player);
void RemovePlayer(const string& game,
                  u_int16_t lolimit,
                  u_int16_t hilimit,
                  const string& player);
                 

END_NAMESPACE(Lounge)

#endif