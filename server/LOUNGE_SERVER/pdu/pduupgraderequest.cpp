/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduUpgradeRequest
  Created  : 04/23/2000

  OVERVIEW : Upgrade Request PDU.

             When the lounge server detects that the client has old
             versions of files, it sends Upgrade Request PDU telling 
             the client where to download new versions of the files.

 ****************************************************************************/

#include "pduupgraderequest.h"
#include "lounge/player.h"
#include "sys.h"


CpduUpgradeRequest::CpduUpgradeRequest()
{
    type_ = PDU_UpgradeRequest;
    length_ = PDU_HEADER_SIZE + sizeof(u_int32_t);
    stuffHeader();
}

//
// The upgrade request consists of the header + the
// server's config file.
//
int CpduUpgradeRequest::sendUpgradeRequest(SOCKET sd,
                                           const char* configFile)
{
    int rc = 0;
    long size = 0;

    // First compute config file size
    FILE* fp = fopen(configFile, "rb");
    if (!fp)
    {
        char s[128];
        sprintf(s, "Can't open config file: %s", configFile);
        Sys_LogError(s);
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) == 0)
    {
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }

    // Can send at max USHRT_MAX bytes of data
    if (size >= USHRT_MAX)
    {
        Sys_LogError("PduUpgradeRequest: size >=USHRT_MAX");
        return 0;
    }

    char* buf = new char[size];
    if (buf == NULL)
    {
        char s[128];
        sprintf(s, "CpduUpgradeRequest: Out of memory");
        Sys_LogError(s);
        return -1;
    }

    //
    // First send the header - the client can handle
    // the pdu coming in pieces.
    //
    u_int32_t s = htonl(size);
    memcpy(value_ + PDU_HEADER_SIZE, &s, sizeof(u_int32_t));
    length_ += size;
    int extraBytes = size;
    stuffHeader();

    // Send header + size
    rc = Sys_send(sd, value_, PDU_HEADER_SIZE + sizeof(u_int32_t));

    //
    // Then stuff the config file to a buffer and send
    // it over
    //

    if (buf != 0)
    {
        long ofs = 0;

        while (size > 0)
        {
            long nread = fread(&buf[ofs], 1, 4096, fp);
            
            if (nread == 0)
                break;

            ofs += nread;
            size -= nread;
        }

        fclose(fp);
        fp = NULL;

        rc = Sys_send(sd, buf, extraBytes);
    }

    delete [] buf;
    buf = 0;
    
    return rc;
}

