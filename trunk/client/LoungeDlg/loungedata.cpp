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
#include "loungedlg/resource.h"
#include "loungedlg/loungedlg.h"
#include "common/stringoutput.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BEGIN_NAMESPACE0()
USING_NS_T(Lounge, Games);

  // Global list of games
  Games g_GamesRealMoney,
        g_GamesPlayMoney;

  Games& GetCurrentGames()
  {
    if (!CLoungeDlg::InPlayMoney())
      return g_GamesRealMoney;
    else
      return g_GamesPlayMoney;
  }

END_NAMESPACE0()


BEGIN_NAMESPACE(Lounge)
USING_NS_T(std, find);


Games& GetGames()
{
  //return g_Games;
  return GetCurrentGames();
}

BOOL IsValid(const Game& g)
{
  return g.name_.size() > 0;
}

BOOL IsValid(const Queue& q)
{
  return q.lolimit_.getDollars() != 0xffff;
}


string Queue::getTitle() const
{
  CStrOut buf;
  if (lolimit_ == 0)
    buf << "Freeroll";
  if (lolimit_ == hilimit_)
    buf << lolimit_;
  else
    buf << lolimit_ << '-' << hilimit_;
  return string(buf.str());
}



// Assume: game names are unique, ie, "Texas Hold'em" is
// only once in the list. It is still possible to add eg
// "Texas Hold'em low" and "Texas Hold'em high".
void AddGame(const string& name)
{
  // Add new game to list unless it's
  // already there.
  Games& games = GetGames();
  Game g(name);
  Games::iterator pos = find(games.begin(), games.end(), g);

  if (pos == games.end())
    games.push_back(Game(name));
}


void AddQueue(const string& gamename,
              u_int16_t queueNumber,
              const CChips& lolimit,
              const CChips& hilimit)
{
  Games& games = GetGames();
  // Find game 'gamename'
  Games::iterator pos = find(games.begin(),
                             games.end(),
                             Game(gamename));

  if (pos == games.end())
  { // Game not seen yet - add it
    Game g(gamename);
    games.push_back(g);
    pos = find(games.begin(), games.end(), Game(gamename));
  }

  if (pos != games.end())
  {
    Game& game = *pos;

    Queue q(queueNumber, lolimit, hilimit);

    // Add it unless it's already there
    if (find(game.queues_.begin(),
             game.queues_.end(), q) == game.queues_.end())
    {
      game.queues_.push_back(q);
    }
  }
  else
  {
    TRACE1("*** AddQueue: Game %s does not exist\n",
           gamename.c_str());
  }
}

void AddPlayerToQueue(Queue& queue,
                      const string& player)
{
  // User should not be in the queue already!
#ifdef _DEBUG
  if (find(queue.players_.begin(),
           queue.players_.end(),
           Player(player)) != queue.players_.end())
  {
    CString s;
    s.Format("add_player: %s already in queue!\n", player.c_str());
    PDU_DEBUG((LPCSTR)s);
  }
#endif

  queue.players_.push_back(Player(player));

  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
    pDlg->addPlayerToQueue(queue.queueNumber_, player.c_str());
}


void RemovePlayerFromQueue(Queue& queue,
                           const string& player)
{
  // Find player..
  Players::iterator p = find(queue.players_.begin(),
                             queue.players_.end(),
                             Player(player));
  if (p != queue.players_.end())
  { // and remove
    queue.players_.erase(p);
  }
  else
  {
    CString s;
    s.Format("remove_player: %s not in queue!\n", player.c_str());
    PDU_DEBUG((LPCSTR)s);
  }

  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
    pDlg->removePlayerFromQueue(queue.queueNumber_, player.c_str());
}


static Game dummy_game;

Game& GetGame(const string& game)
{
  Games& games = GetGames();
  // Find game 'gamename'
  Games::iterator pos = find(games.begin(),
                             games.end(),
                             Game(game));

  if (pos != games.end())
  {
    return *pos;
  }

  return dummy_game;
}

Game& GetGameByQueue(USHORT queuenumber)
{
  Games& games = GetGames();
  Games::iterator git = games.begin(),
                  gend = games.end();

  for (; git != gend; ++git)
  {
    for (Queues::iterator qit = git->queues_.begin(),
                          qend = git->queues_.end();
         qit != qend;
         ++qit)
    {
      if (qit->queueNumber_ == queuenumber)
        return *git;
    }
  }

  return dummy_game;
}

Game& GetGameByNumber(USHORT game_number)
{
  Games& games = GetGames();
  // Find the game_number'th game
  Games::iterator git = games.begin(),
                  gend = games.end();
  for ( ; game_number > 0 && git != gend; ++git)
  {
    --game_number;
  }

  if (game_number == 0 && git != gend)
  {
    return *git;
  }

  return dummy_game;
}

// Return the game the has table 'tablenumber'
Game& GetGameByTable(USHORT tablenumber)
{
  Games& games = GetGames();
  for (Games::iterator gi = games.begin(),
                       ge = games.end();
       gi != ge;
       ++gi)
  {
    for (Queues::iterator qi = gi->queues_.begin(),
                          qe = gi->queues_.end();
         qi != qe;
         ++qi)
    {
      Queue& queue = (*qi);
      for (Tables::iterator ti = queue.tables_.begin(),
                            te = queue.tables_.end();
           ti != te;
           ++ti)
      {
        if (ti->number_ == tablenumber)
          return *gi;
      }
    }
  }

  return dummy_game;
}


static Queue dummy_queue;

// Return the queue with 'number' in 'game'.
Queue& GetQueue(const string& game, u_int16_t number)
{
  Games& games = GetGames();
  // Find game 'gamename'
  Games::iterator pos = find(games.begin(),
                             games.end(),
                             Game(game));

  if (pos != games.end())
  {
    Game& game = *pos;
    return GetQueue(game, number);
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}


Queue& GetQueue(Game& game, u_int16_t number)
{
  // Find the queue with 'number'
  Queues::iterator pos = find(game.queues_.begin(),
                              game.queues_.end(),
                              number);
  if (pos != game.queues_.end())
  {
    return *pos;
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}

Queue& GetQueue(Game& game,
                const CChips& lolimit,
                const CChips& hilimit)
{
  // Find the queue
  Queue q(0xffff, lolimit, hilimit);
  Queues::iterator pos = find(game.queues_.begin(),
                              game.queues_.end(), q);
  if (pos != game.queues_.end())
  {
    return *pos;
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}

Queue& GetQueue(u_int16_t queueNumber)
{
  Games& games = GetGames();

  // iterate over games, return queue with 'queueNumber'

  for (Games::iterator i = games.begin(),
                       e = games.end();
       i != e;
       ++i)
  {
    Queue& q = GetQueue(*i, queueNumber);
    if (q.lolimit_ != 0xffff &&
        q.queueNumber_ == queueNumber)
    {
      return q;
    }
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}

Queue& GetQueueIndex(const string& game, int index)
{
  Games& games = GetGames();

  // Find game 'gamename'
  Games::iterator pos = find(games.begin(),
                             games.end(),
                             Game(game));

  if (pos != games.end())
  {
    Game& game = *pos;
    return GetQueueIndex(game, index);
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}


Queue& GetQueueIndex(Game& game, int queue_index)
{
  // Find the 'index'th queue in 'game'
  Queues::iterator qit = game.queues_.begin(),
                   qend = game.queues_.end();
  for ( ; queue_index > 0 && qit != qend; ++qit)
  {
    --queue_index;
  }

  if (queue_index == 0 && qit != qend)
  {
    return *qit;
  }

  dummy_queue.lolimit_ = 0xffff;
  dummy_queue.hilimit_ = 0xffff;
  return dummy_queue;
}


void AddTableToQueue(Queue& queue,
                     const string& table)
{

    Table t(table);

  // Table should not be in the queue already!
#ifdef _DEBUG
  if (find(queue.tables_.begin(),
           queue.tables_.end(), t) != queue.tables_.end())
  {
    CString s;
    s.Format("add_table: %s already in queue!\n", table.c_str());
    PDU_DEBUG((LPCSTR)s);
  }
#endif

  queue.tables_.push_back(t);

  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
    pDlg->addTableToQueue(queue.queueNumber_, table.c_str());
}


void RemoveTableFromQueue(Queue& queue,
                          const string& table)
{
  // Find table
  Tables::iterator p = find(queue.tables_.begin(),
                            queue.tables_.end(),
                            Table(table));
  if (p != queue.tables_.end())
  { // and remove
    queue.tables_.erase(p);
  }
  else
  {
    CString s;
    s.Format("remove_table: %s not in queue!\n", table.c_str());
    PDU_DEBUG((LPCSTR)s);
  }

  // XXX refresh queue if displayed
  CLoungeDlg* pDlg = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  if (pDlg)
    pDlg->removeTableFromQueue(queue.queueNumber_, table.c_str());
}

void EmptyAll()
{
  GetGames().clear();
}


END_NAMESPACE(Lounge)