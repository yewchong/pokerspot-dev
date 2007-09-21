#include "stdafx.h"
#pragma warning( disable : 4786 )
#include "floormap.h"
#include "loungedata.h"
#include "resource.h"
#include "loungedlg.h"
#include "ui/cmdid.h"
#include "common/chips.h"
#include <map>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  CFloorMap g_FloorMap;

  using std::map;
  using std::string;
  // Map game type to game name
  typedef map<u_int16_t, string> GameTypeToName;
  GameTypeToName g_gameTypeToName;

  bool AddGame(u_int16_t gameType, const string& name)
  {
    g_gameTypeToName.insert(GameTypeToName::value_type(gameType, name));
    return true;
  }

  string GetGameName(u_int16_t gameType)
  {
    GameTypeToName::iterator p = g_gameTypeToName.find(gameType);
    if (p != g_gameTypeToName.end())
    {
        return (*p).second;
    }
    
    return "Unknown";
  }

} // namespace


CFloorMap::CFloorMap()
{}


CFloorMap* CFloorMap::Instance()
{
  return &g_FloorMap;
}


void CFloorMap::Invalidate()
{
}


void CFloorMap::disconnect()
{
  ASSERT(AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CLoungeDlg)));
  static_cast<CLoungeDlg*>(AfxGetMainWnd())->disconnect();
  g_gameTypeToName.clear();
}


void CFloorMap::setStats(int players, int tables)
{
  ASSERT(AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CLoungeDlg)));
  static_cast<CLoungeDlg*>(AfxGetMainWnd())->setStats(players, tables);
}

void CFloorMap::addGame(u_int16_t gameType,
                        const string& name)
{
  AddGame(gameType, name);
}

void CFloorMap::addQueue(u_int16_t gametype,
                         u_int16_t number,
                         u_int16_t players,
                         const CChips& lolimit,
                         const CChips& hilimit)
{
  Lounge::AddQueue(GetGameName(gametype), number, lolimit, hilimit);
//  Lounge::AddTable(GetGameName(gametype), number, lolimit, hilimit);

  TRACE3("Added queue: %d %d %d ", gametype, number, players);
  TRACE2("%f %f\n", lolimit.asDouble(), hilimit.asDouble());
}


void CFloorMap::recalcLayout()
{
  CLoungeDlg* pLD = static_cast<CLoungeDlg*>(AfxGetMainWnd());
  pLD->resetList();
  pLD->addGamesTabs();
  pLD->populateList(0);
}


void CFloorMap::setTitle()
{
}

//
// Find the table specified by 'number' and
// return its limits.
//
void CFloorMap::getTableLimits(USHORT number,
                               CChips& lolimit,
                               CChips& hilimit,
                               USHORT& isHiLo,
                               USHORT& gameType)
{
  using namespace Lounge;

  const Games& games = Lounge::GetGames();
  Games::const_iterator git = games.begin(),
                        gend = games.end();
  
  while (git != gend)
  {
    for (Queues::const_iterator qi = git->queues_.begin(),
                                qe = git->queues_.end();
         qi != qe;
         ++qi)
    {
      const Queue& queue = (*qi);
      for (Tables::const_iterator i = queue.tables_.begin(),
                                  e = queue.tables_.end();
           i != e;
           ++i)
      {
        if (i->number_ == number)
        {
          gameType = git->gameType_;
          isHiLo = git->isHiLoSplit();
          string s = git->name_;
          lolimit = queue.lolimit_;
          hilimit = queue.hilimit_;
          return;
        }
      }
    }
    ++git;
  }
  
  ASSERT(FALSE); // NOT REACHED
}


// Upon joining a table, change buttons from "Remove" to
// "Add Name" to allow joining to other tables with the
// same limits.
void CFloorMap::joinedTable(USHORT tablenumber,
                            const CChips& lo,
                            const CChips& hi,
                            ULONG launch_id)
{
  Lounge::Game& g = Lounge::GetGameByTable(tablenumber);
  ASSERT(g.name_.size() > 0);

  if (g.name_.size() > 0)
  {
    Lounge::Queue& q = Lounge::GetQueue(g, lo, hi);
    ASSERT(q.lolimit_ == lo);

    if (q.lolimit_ == lo)
    {
      q.inQueue_ = FALSE; // joined ==> not in queue any more!
      ASSERT(AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CLoungeDlg)));
      CLoungeDlg* pLD = static_cast<CLoungeDlg*>(AfxGetMainWnd());
      pLD->refreshButtons();
    }
  }
}


// The launching of the table client should never fail,
// but if it happens, turn button back to "Add Name"
// anyway.
void CFloorMap::launchFailed(USHORT tablenumber,
                             const CChips& lo,
                             const CChips& hi)
{
  Lounge::Game& g = Lounge::GetGameByTable(tablenumber);
  ASSERT(g.name_.size() > 0);

  if (g.name_.size() > 0)
  {
    Lounge::Queue& q = Lounge::GetQueue(g, lo, hi);
    ASSERT(q.lolimit_ == lo);

    if (q.lolimit_ == lo)
    {
      q.inQueue_ = FALSE; // not in queue any more!
      ASSERT(AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CLoungeDlg)));
      static_cast<CLoungeDlg*>(AfxGetMainWnd())->refreshButtons();
    }
  }
}