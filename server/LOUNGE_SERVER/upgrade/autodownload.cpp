#if 0 - this file not used

#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include "pdu.h"
#include "includes.h"
#include "compute_md5_checksum.h"

#define CHECKSUM_SIZE 16
#define CONFIGURATION_FILE "client_versions.conf"

int send_upgraderequest_pdu(int sockfd, const char* conf_file);

/*
 * C-callable entry point to the automatic
 * upgrade implementation.
 *
 * Returns: 0 if client was up to date
 *          1 if client requires upgrading
 *          -1 on error
 *
 */
extern "C"
int check_autoupgrade(int sockfd)
{
  int rc = 0;
  u_byte_t client_checksum[CHECKSUM_SIZE];
  memset(client_checksum, 0, CHECKSUM_SIZE);

  // Read the 16-byte configuration checksum
  // sent by the client.
  int nread = recv(sockfd, (char*)&client_checksum, CHECKSUM_SIZE, 0);
  if (nread < 0)
  {
    perror("process_clientconfiguration: recv client checksum");
    return -1;
  }

  // Read current version info at server.
  u_byte_t server_checksum[CHECKSUM_SIZE];
  if (compute_md5_checksum(CONFIGURATION_FILE, server_checksum) == -1)
  {
    perror("process_clientconfiguration: digest_file CONFIGURATION_FILE");
    return -1;
  }

  // If the checksums differ, send current
  // configuration file to client as an
  // upgrade request.
  if (strcmp((const char*)client_checksum,
             (const char*)server_checksum) != 0)
  {
    if (send_upgraderequest_pdu(sockfd, CONFIGURATION_FILE) != -1)
      rc = 0; // Client needs upgrading
    else
      rc = -1;// Error
  }
  else
    rc = 1; // Client is up to date.

  return rc;
}


int send_upgraderequest_pdu(int sd, const char* conf_file)
{
  typedef struct pdu_header pdu_header_t;
  int rc = 0;
  char* buf = NULL;
  long size = 0;
  long outlength = sizeof(pdu_header_t);

  FILE* fp = fopen(conf_file, "rb");
  if (!fp)
  {
    perror("fopen");
    return -1;
  }

  if (fseek(fp, 0, SEEK_END) == 0)
  {
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Put both PDU header and config file data to 'buf'
    buf = new char[sizeof(pdu_header_t) + size];
    outlength += size;

    // Can send at max USHRT_MAX bytes of data
    assert(outlength < USHRT_MAX);    

    if (buf != 0)
    {
      long ofs = sizeof(pdu_header_t),
           chunk = 4096;

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
    }
  }

  if (buf != 0)
  {
    pdu_header_t pduh;
    memset(&pduh, 0, sizeof(pdu_header_t));
#define PDU_TYPE_UPGRADEREQUEST 506 // XXX add to pdu.h
    pduh.type = htons(PDU_TYPE_UPGRADEREQUEST);
    pduh.length = htons((u_short)outlength);
    memcpy(buf, &pduh, sizeof(pdu_header_t));

    rc = send(sd, buf, outlength, 0);
  }

  return rc;
}

#endif