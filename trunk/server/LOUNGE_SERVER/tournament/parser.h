#ifndef __lounge_parser_h__
#define __lounge_parser_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CParser
  Created  : 08/04/2000

  OVERVIEW : This class implements the parsing of the tournament script
             and parameterizing the tournament state objects accordingly.

 ****************************************************************************/

class CTournament;
class CTournamentState;

#include <fstream.h>
#include <string>
#include <vector>
#include <algorithm>
#include "compat.h"
USING_NS_T(std, string);
USING_NS_T(std, istream_iterator);
USING_NS_T(std, vector);

#ifdef _WIN32
//using namespace std;
#endif
//typedef istream_iterator<string, char, char_traits<char> > string_input;
typedef istream_iterator<string, char> string_input;
typedef vector<string> strvector;

class CParser
{
public:
    CParser(CTournament*);

    bool loadScript(const char* fname);

private:
    bool interpret(strvector&);

    bool commentToken(strvector::const_iterator&,
                      const strvector::const_iterator&);
    bool gameToken(strvector::const_iterator&,
                   const strvector::const_iterator&);
    bool freezeToken(strvector::const_iterator&,
                     const strvector::const_iterator&);
    bool timeToken(strvector::const_iterator&,
                   const strvector::const_iterator&);
    bool tableToken(strvector::const_iterator&,
                    const strvector::const_iterator&);

    CTournament* tournament_;
    CTournamentState* state_;
};

#endif