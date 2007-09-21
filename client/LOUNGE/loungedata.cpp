/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : loungedata.cpp
  Created  : 02/06/2000

  OVERVIEW : Data structure definitions for maintaining the
             lounge data structures.

             The lounge data is essentially a list (games) of
             lists (queues) of lists (players).

 ****************************************************************************/

#include "stdafx.h"
#include "loungedata.h"
#include <algorithm>


BEGIN_NAMESPACE0()

  // Global list of games
  Games g_Games;

END_NAMESPACE0()


BEGIN_NAMESPACE(Lounge)

USING_NS_T(std, find);

void AddGame(const string& name)
{
  g_Games.push_back(Game(name));
}


void AddQueue(const string& gamename,
              u_int16_t lolimit,
              u_int16_t hilimit)
{
  // Find game 'gamename'
  Games::iterator pos = find(g_Games.begin(),
                             g_Games.end(),
                             Game(gamename));

  if (pos != g_Games.end())
  {
    Game& game = *pos;
    game.queues_.push_back(Queue(lolimit, hilimit));
  }
  else
  {
    TRACE1("*** AddQueue: Game %s does not exist\n",
           gamename.c_str());
  }
}

void AddPlayer(const string& game,
               u_int16_t lolimit,
               u_int16_t hilimit,
               const string& player)
{
  // Find game 'gamename'
  Games::iterator pos = find(g_Games.begin(),
                             g_Games.end(),
                             Game(gamename));

  if (pos != g_Games.end())
  {
    Game& game = *pos;

    // Find queue
    Queues::iterator q = find(game.queues_.begin(),
                              game.queues_.end(),
                              Queue(lolimit, hilimit));

    if (q != game.queues_.end())
    {
      q->push_back(Player(player));
    }
    else
    {
      TRACE3("*** AddPlayer: Game %s does not have queue %d-%d\n",
           gamename.c_str(), lolimit, hilimit);
    }
  }
  else
  {
    TRACE1("*** AddPlayer: Game %s does exist\n",
           gamename.c_str());
  }
}


void RemovePlayer(const string& gamename,
                  u_int16_t lolimit,
                  u_int16_t hilimit,
                  const string& player)
{
  // Find game 'gamename'
  Games::iterator pos = find(g_Games.begin(),
                             g_Games.end(),
                             Game(gamename));

  if (pos != g_Games.end())
  {
    Game& game = *pos;

    // Find queue
    Queues::iterator q = find(game.queues_.begin(),
                              game.queues_.end(),
                              Queue(lolimit, hilimit));

    if (q != game.queues_.end())
    {
      Queue& queue = *q;

      // Find player
      Players::iterator p = find(queue.begin(),
                                 queue.end(),
                                 Player(player));
      if (p != queue.end())
      {
        queue.erase(p);
      }
    }
  }
}
