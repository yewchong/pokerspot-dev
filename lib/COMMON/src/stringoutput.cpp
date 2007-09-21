/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : class CStringOutput
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style string output (instead of
             using sprintf).

 ****************************************************************************/


#include "common/stringoutput.h"


CStringOutput::CStringOutput()
  :
  ends_(false)
{
  outstr_.precision(2);
}

CStringOutput::~CStringOutput()
{
  outstr_.rdbuf()->freeze(0);
}

