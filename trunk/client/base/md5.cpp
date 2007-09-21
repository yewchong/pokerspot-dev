/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : md5.cpp
  Created  : 01/29/2000

  OVERVIEW : A routine for computing the MD5 checksum
             for a file.

 ****************************************************************************/

/* The following makes MD default to MD5 if it has not already been
  defined with C compiler flags.
 */
#include "stdafx.h"

#ifndef MD
#define MD MD5
#endif

#include "base/md5.h"
#include <stdio.h>
#include <time.h>
#include "md5/global.h"
#include "md5/md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Map generic names to MD5
#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final



BEGIN_NAMESPACE(Base)

/* 
 * Digests a file and returns the MD5 checksum in
 * 16-byte 'outdigest'.
 * Returns 0 on success, -1 otherwise.
 */
int ComputeMD5Checksum(const char* filename,
                       u_byte_t* outdigest)
{
  int rc = 0;
  FILE* file = 0;
  MD_CTX context;
  int len = 0;
  const int chunk = 4 * 1024;
  unsigned char buffer[chunk], digest[16];

  if ((file = fopen(filename, "rb")) == NULL)
  {
    // printf ("%s can't be opened\n", filename);
    rc = -1;
  }
  else
  {
    MDInit(&context);

    while (len = fread(buffer, 1, chunk, file))
      MDUpdate(&context, buffer, len);

    MDFinal(digest, &context);

    fclose (file);
    file = 0;

    memcpy(outdigest, digest, 16);
  }

  return rc;
}


END_NAMESPACE(Base)