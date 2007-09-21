#ifndef __psi_nocasecompare_h__
#define __psi_nocasecompare_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : 
  Created  : 08/06/2000

  OVERVIEW : Functions for doing case insensitive string compares.

 ****************************************************************************/

#include <string>


#ifndef _WIN32
inline int strcmpi(const char* s1, const char* s2)
{
    return strcasecmp(s1, s2);
}
#endif


inline bool nocasecompare(const string& s1, const char* c1)
{
    return strcmpi(s1.c_str(), c1) == 0;
}

inline bool nocasecompare(const string& s1, const string& s2)
{
    return strcmpi(s1.c_str(), s2.c_str()) == 0;
}


#endif