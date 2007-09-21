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
#include "common/chips.h"

BEGIN_NAMESPACE(Lounge)

#ifdef _WIN32
using std::list;
using std::string;
#endif

//--------------------- 
// A player in a queue
//
struct Player
{
  Player() {}
  Player(const Player& p)
    : name_(p.name_) {}
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
// One table
//
struct Table
{
  Table()
    : number_(0)
  {}
  Table(const Table& q)
    : title_(q.title_), number_(q.number_)
  {}
  Table(const string& title)
    : title_(title), number_(0)
  {
    if (title_.size() > 0)
    {
      char buf[100];
      sscanf(title_.c_str(), "%s %d", buf, &number_);
    }
  }
  Table& operator = (const Table& rhs)
  {
    if (this != &rhs)
    {
      title_ = rhs.title_;
      number_ = rhs.number_;
    }
    return *this;
  }

  string getTitle() const
  {
    return title_;
  }

  u_int16_t getNumber() const
  {
    return number_;
  }

  string title_;
  u_int16_t number_;
};

inline
bool operator == (const Table& lhs, const Table& rhs)
{
  return lhs.number_ == rhs.number_;
}

typedef list<Table> Tables;


//---------------------
// One queue in a game
//
struct Queue
{
  Queue()
    : queueNumber_(0), lolimit_(0), hilimit_(0), inQueue_(FALSE) {}
  Queue(const Queue& q)
    : queueNumber_(q.queueNumber_),
      lolimit_(q.lolimit_),
      hilimit_(q.hilimit_),
      players_(q.players_),
      inQueue_(FALSE) {}
  Queue(u_int16_t number, const CChips& lo, const CChips& hi)
    : queueNumber_(number), lolimit_(lo), hilimit_(hi), inQueue_(FALSE) {}
  Queue& operator = (const Queue& rhs)
  {
    if (this != &rhs)
    {
      queueNumber_ = rhs.queueNumber_;
      lolimit_ = rhs.lolimit_;
      hilimit_ = rhs.hilimit_;
      players_ = rhs.players_;
      inQueue_ = rhs.inQueue_;
    }
    return *this;
  }

  string getTitle() const;

  u_int16_t queueNumber_;
  CChips lolimit_;
  CChips hilimit_;  
  Players players_;
  BOOL inQueue_;
  Tables tables_;
};


inline
bool operator == (const Queue& lhs, const Queue& rhs)
{
  return lhs.lolimit_ == rhs.lolimit_ &&
         lhs.hilimit_ == rhs.hilimit_;
}

inline
bool operator == (const Queue& lhs, u_int16_t queueNumber)
{
  return lhs.queueNumber_ == queueNumber;
}

inline
bool operator == (u_int16_t queueNumber, const Queue& rhs)
{
  return rhs.queueNumber_ == queueNumber;
}


typedef list<Queue> Queues;

enum GameType
{
  GT_Holdem     = 0,
  GT_SevenStud  = 1,
  GT_Omaha      = 2
};


//-----------------------
// A game maintains list
// of queues.
struct Game
{
  Game() {}
  Game(const Game& g)
    : gameType_(g.gameType_), name_(g.name_), queues_(g.queues_)
  {}
  Game(const string& name)
    : gameType_(GT_Holdem), name_(name)
  {
    decideGameType();
  }

  Game& operator = (const Game& rhs)
  {
    if (this != &rhs)
    {
      gameType_ = rhs.gameType_;
      name_ = rhs.name_;
      queues_ = rhs.queues_;
    }
    return *this;
  }

  void decideGameType()
  {
    if (name_.find("Hold'em") != string::npos)
      gameType_ = GT_Holdem;
    else if (name_.find("Stud") != string::npos)
      gameType_ = GT_SevenStud;
    else if (name_.find("Omaha") != string::npos)
      gameType_ = GT_Omaha;
  }

  USHORT isHiLoSplit() const
  {
    if (name_.find("Hi/Lo") != string::npos)
      return 1;
    else
      return 0;
  }
  
  u_int16_t gameType_;
  string name_;
  Queues queues_;
};

inline
bool operator == (const Game& lhs, const Game& rhs)
{
  return lhs.name_ == rhs.name_;
}

typedef list<Game> Games;

//------------
// Simple Lounge data API for searching for games,
// queues & tables by various criteria.
// 

Games& GetGames           ();
Game&  GetGame            (const string& game);
Game&  GetGameByTable     (USHORT table_number);
Game&  GetGameByNumber    (USHORT game_number);
Game&  GetGameByQueue     (USHORT queue_number);
BOOL   IsValid            (const Game&);

Queue& GetQueue           (const string& game,
                           u_int16_t queueNumber);
Queue& GetQueue           (Game& game,
                           u_int16_t queueNumber);
Queue& GetQueue           (Game& game,
                           const CChips& lolimit,
                           const CChips& hilimit);
Queue& GetQueueIndex      (const string& game,
                           int index);
Queue& GetQueueIndex      (Game& game,
                           int index);
Queue& GetQueue           (u_int16_t queueNumber);


BOOL   IsValid            (const Queue&);

Table& GetFirstTable      (const string& game,
                           const Queue& queue);

void AddGame              (const string& name);
void AddQueue             (const string& game,
                           u_int16_t queuenumber,
                           const CChips& lolimit,
                           const CChips& hilimit);
void AddPlayerToQueue     (Queue& queue,
                           const string& player);
void RemovePlayerFromQueue(Queue& queue,
                           const string& player);
void AddTableToQueue      (Queue& queue,
                           const string& table);
void RemoveTableFromQueue (Queue& queue,
                           const string& table);
void EmptyAll();


END_NAMESPACE(Lounge)

#endif