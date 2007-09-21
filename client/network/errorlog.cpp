/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : errorlog.h
  Created  : 08/14/2000

  OVERVIEW : Error logging facility for the client.

 ****************************************************************************/

#include "stdafx.h"
#include "network/errorlog.h"

BEGIN_NAMESPACE(Network)

void LogError(const char* logmsg)
{
#ifdef PSPOT_TABLE_MODULE_
    const char* fname = "errorlog-table.txt";
#else
    const char* fname = "errorlog-lounge.txt";
#endif

    // Record the last error
    FILE* fp = fopen(fname, "w");
    if (fp)
    {
        time_t t = 0;
        time(&t);
        struct tm* gmt = gmtime(&t);

        fprintf(fp, asctime(gmt));
        fprintf(fp, logmsg);
        fprintf(fp, "\n");

        fclose(fp);
        fp = NULL;
    }
}

END_NAMESPACE(Network)
