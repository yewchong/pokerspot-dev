#ifndef __base_md5_h__
#define __base_md5_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : md5.h
  Created  : 01/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "network/ntwktype.h"
#include "network/compat.h"

BEGIN_NAMESPACE(Base)

extern
int ComputeMD5Checksum(const char* filename,
                       u_byte_t* outdigest);

END_NAMESPACE(Base)


#endif