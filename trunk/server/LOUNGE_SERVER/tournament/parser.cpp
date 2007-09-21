/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CParser
  Created  : 08/04/2000

  OVERVIEW : This class implements the parsing of the tournament script
             and parameterizing the tournament state objects accordingly.

 ****************************************************************************/

#include "tournament/parser.h"
#include "tournament/tournament.h"
#include "tournament/tournamentstate.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/action.h"
#include "tournament/event.h"
#include "tournament/runstate.h"
#include "nocasecompare.h"
#include "common/chips.h"
#include <stdio.h>

istream& eatwhite(ifstream& ifs)
{
#ifdef _WIN32

    ifs.eatwhite();
    return ifs;

#else

    char c;
    while (ifs.get(c))
    {
        if (isspace(c) == 0)
        {
            ifs.putback(c);
            break;
        }
    }
    return ifs;

#endif
}

CParser::CParser(CTournament* t)
    :
    tournament_(t)
{}


bool CParser::loadScript(const char* fname)
{
  if (!tournament_) return false;

  //printf("CParser::loadScript\n");
  tournament_->dumpEvents();
  tournament_->clearEvents();

  strvector data;
  ifstream ifs(fname);

  if (!ifs.is_open())
  {
      char* s = new char[100 + strlen(fname)];
      sprintf(s, "Unable to open tournament script file: %s", fname);
      tournament_->setDescription(s);
      delete [] s;
      return false;
  }

  // Read the file contents
  //copy(string_input(ifs), string_input(), back_inserter(data));
  while (!ifs.eof())
  {
      eatwhite(ifs);
      if (ifs.eof())
          break;

      string s;
      ifs >> s;
      if (s == "END")
      {
          // XXX: work-around to a bug in
          // Microsoft ifstream - use end of file marker!
          break;
      }
      data.push_back(s);
  }

  if (data.size() < 2)
  {
      char* s = new char[100 + strlen(fname)];
      sprintf(s, "Invalid tournament script: %s", fname);
      tournament_->setDescription(s);
      delete [] s;
      return false;
  }

  bool rc = interpret(data);
  tournament_->dumpEvents();

  return rc;
}


bool CParser::interpret(strvector& data)
{
  bool rc = true;

  for (strvector::const_iterator i = data.begin(),
                                 end = data.end();
       i != end && rc; )
  {
    string s = *i++;
    if (s == "/*")
        rc = commentToken(i, end);
    else if (nocasecompare(s, "game"))
        rc = gameToken(i, end);
    else if (nocasecompare(s, "freeze"))
        rc = freezeToken(i, end);
    else if (nocasecompare(s, "at"))
        rc = timeToken(i, end);
  }

  return rc;
}

bool CParser::gameToken(strvector::const_iterator& i,
                        const strvector::const_iterator& end)
{
    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid game token(1)");
        return false;
    }

    string s = *i++;

    if (nocasecompare(s, "Holdem"))
    {
        tournament_->setGameType(GT_Holdem);
        tournament_->setIsHiLo(false);
    }
    else if (nocasecompare(s, "Holdem_Hi/Lo"))
    {
        tournament_->setGameType(GT_Holdem);
        tournament_->setIsHiLo(true);
    }
    else if (nocasecompare(s, "Omaha"))
    {
        tournament_->setGameType(GT_Omaha);
        tournament_->setIsHiLo(false);
    }
    else if (nocasecompare(s, "Omaha_Hi/Lo"))
    {
        tournament_->setGameType(GT_Omaha);
        tournament_->setIsHiLo(true);
    }
    else if (nocasecompare(s, "7-Stud"))
    {
        tournament_->setGameType(GT_SevenStud);
        tournament_->setIsHiLo(false);
    }
    else if (nocasecompare(s, "7-Stud_Hi/Lo"))
    {
        tournament_->setGameType(GT_SevenStud);
        tournament_->setIsHiLo(true);
    }
    else
    {
        tournament_->setDescription(
            "Syntax error: invalid game type.\r\nValid values are:\r\n"
            "Holdem, Holdem_Hi/Lo, "
            "Omaha, Omaha_Hi/Lo, "
            "7-Stud, 7-Stud_Hi/Lo");
        return false;
    }

    return true;
}

bool CParser::freezeToken(strvector::const_iterator& i,
                          const strvector::const_iterator& end)
{
    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid freeze token");
        return false;
    }

    string s = *i++;
    int freezeTime = atoi(s.c_str());

    if (i != end)
    {
        s = *i++;
        if (nocasecompare(s, "mins"))
            freezeTime *= 60;
        else if (!nocasecompare(s, "secs"))
        {
            tournament_->setDescription("Syntax error: invalid freeze token: 'mins' or 'secs' expected");
            return false;
        }
    }

    tournament_->setFreezeTime(freezeTime);

    return true;
}

bool CParser::timeToken(strvector::const_iterator& i,
                        const strvector::const_iterator& end)
{
    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid time token(1)");
        return false;
    }

    string s = *i++;
    int t = atoi(s.c_str());

    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid time token(2)");
        return false;
    }

    s = *i++;

    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid time token(3)");
        return false;
    }

    if (nocasecompare(s, "mins"))
    {
        s = *i++;
        t = 60 * t;
    }
    else if (nocasecompare(s, "secs"))
    {
        s = *i++;
    }

    // do limits

    if (nocasecompare(s, "limits"))
    {
        if (i == end)
        {
            tournament_->setDescription("Syntax error: invalid limits token(4)");
            return false;
        }

        s = *i++;
        double d = atof(s.c_str());
        if (d > 0.0)
        {
          CChips lolimit(d);

          CTimedEvent* e = new CTimedEvent(t, new CLimitAction(lolimit));
          tournament_->addEvent(e);
        }
    }
    else if (nocasecompare(s, "limit"))
    {
        // change to limit betting

        CTimedEvent* e = new CTimedEvent(t, new CBetStructureAction(BS_Limit));
        tournament_->addEvent(e);
    }
    else if (nocasecompare(s, "potlimit"))
    {
        // change to pot limit betting

        CTimedEvent* e = new CTimedEvent(t, new CBetStructureAction(BS_PotLimit));
        tournament_->addEvent(e);
    }
    else if (nocasecompare(s, "nolimit"))
    {
        // change to no limit betting

        CTimedEvent* e = new CTimedEvent(t, new CBetStructureAction(BS_NoLimit));
        tournament_->addEvent(e);
    }
    else if (nocasecompare(s, "pause"))
    {
        if (i == end)
        {
            tournament_->setDescription("Syntax error: invalid pause token(4)");
            return false;
        }

        // do pause
        s = *i++;
        int pause = atoi(s.c_str());
        pause = 60 * pause; // store in seconds
        
        // Pause is a PAUSE action at designated time
        // and PLAY action after 'pause' seconds
        CTimedEvent* e = new CTimedEvent(t, new CSyncAction(SF_Pause, pause));
        tournament_->addEvent(e);
        e = new CTimedEvent(pause, new CSyncAction(SF_Play));
        tournament_->addEvent(e);
    }
    else if (nocasecompare(s, "game"))
    {
        if (i == end)
        {
            tournament_->setDescription("Syntax error: invalid game type token(1)");
            return false;
        }
        string s = *i++;

        GameType gt = GT_Holdem;
        bool hilo = false;
        if (nocasecompare(s, "Holdem"))
        {
            gt = GT_Holdem;
            hilo = false;
        }
        else if (nocasecompare(s, "Holdem_Hi/Lo"))
        {
            gt = GT_Holdem;
            hilo = true;
        }
        else if (nocasecompare(s, "Omaha"))
        {
            gt = GT_Omaha;
            hilo = false;
        }
        else if (nocasecompare(s, "Omaha_Hi/Lo"))
        {
            gt = GT_Omaha;
            hilo = true;
        }
        else if (nocasecompare(s, "7-Stud"))
        {
            gt = GT_SevenStud;
            hilo = false;
        }
        else if (nocasecompare(s, "7-Stud_Hi/Lo"))
        {
            gt = GT_SevenStud;
            hilo = true;
        }
        else
        {
            tournament_->setDescription(
                "Syntax error: invalid game type.\r\nValid values are:\r\n"
                "Holdem, Holdem_Hi/Lo, "
                "Omaha, Omaha_Hi/Lo, "
                "7-Stud, 7-Stud_Hi/Lo");
            return false;
        }

        CTimedEvent* e = new CTimedEvent(t, new CGameTypeAction(gt, hilo));
        tournament_->addEvent(e);
    }

    return true;
}

bool CParser::tableToken(strvector::const_iterator& i,
                         const strvector::const_iterator& end)
{
    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid table token(1)");
        return false;
    }

    string s = *i++;
    int tables = atoi(s.c_str());

    if (i == end)
    {
        tournament_->setDescription("Syntax error: invalid table token(2)");
        return false;
    }

    s = *i++;

    if (nocasecompare(s, "sync"))
    {
        CTableEvent* e = new CTableEvent(tables, new CSyncAction(SF_Step));
        tournament_->addEvent(e);
    }
    else
    {
        tournament_->setDescription("Syntax error: invalid table token(3)");
        return false;
    }

    return true;
}


bool CParser::commentToken(strvector::const_iterator& i,
                           const strvector::const_iterator& end)
{
    while (i != end)
    {
        string s = *i++;
        if (s == "*/")
            return true;
    }

    return true;
}
