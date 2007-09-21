#ifndef CB__CHECKS_H
#define CB__CHECKS_H

//
// Copyright (c) 1999 Colosseum Builders, Inc.
// All rights reserved.
//
// Colosseum Builders, Inc. makes no warranty, expressed or implied
// with regards to this software. It is provided as is.
//
// See the README.TXT file that came with this software for restrictions
// on the use and redistribution of this file or send E-mail to
// info@colosseumbuilders.com
//

//
//  Title: Consistency Check Macros
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Date:  May 1, 1999
//

#include <stdexcept>

class AssertionFailure : public std::logic_error
{
public:
  AssertionFailure (const std::string &msg) : std::logic_error (msg) {} 
} ;

// Macro to validate arguments.
#define CHECKARGUMENT(test) if (!(test)) throw AssertionFailure ("Assertion Failure (Bad Parameter): " #test " " __FILE__) ;

// Macro for consistency Checks
#if defined (ENABLECHECKS)
inline void reportinternalerror (const char *msg)
{
  throw AssertionFailure (msg) ; 
}
# define ASSERT(test) if (!(test)) reportinternalerror ("Assertion Failure: " #test " " __FILE__ ) ;
#else
# define ASSERT(test) 
#endif

#endif
