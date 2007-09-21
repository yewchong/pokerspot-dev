#include "pdu.h"
#include <stdio.h>
#include <vector>

#define BUFSIZE 16
#define MODULE_SIZE 16
#define CHECKSUM_SIZE 16

// client sends u_int16_t numentries
// times this:
struct pdu_moduleinfo
{
  u_byte_t module_[MODULE_SIZE];
  u_byte_t checksum_[CHECKSUM_SIZE];

  pdu_moduleinfo()
  {
    memset(module_, 0, sizeof(module_));
    memset(checksum_, 0, sizeof(checksum_));
  }

  pdu_moduleinfo(const char* module,
                 const char* checksum)
  {
    strncpy((char*)module_, module, sizeof(module_));
    strncpy((char*)checksum_, checksum, sizeof(checksum_));
  }

  pdu_moduleinfo(const pdu_moduleinfo_t& p)
  {
    memcpy(module_, p.module_, sizeof(module_));
    memcpy(checksum_, p.checksum_, sizeof(checksum_));
  }

  pdu_moduleinfo& operator = (const pdu_moduleinfo& rhs)
  {
    if (this != &rhs)
    {
      memcpy(module_, rhs.module_, sizeof(module_));
      memcpy(checksum_, rhs.checksum_, sizeof(checksum_));
    }
    return *this;
  }
};

// Each ModuleEntry specifies the module
// name and checksum and the ftp address
// from where to download the latest version.
struct ModuleEntry
{
  pdu_moduleinfo moduleinfo_;
  string         ftpaddr_;

  ModuleEntry()
  {
    // data members have default ctors
  }

  ModuleEntry(const ModuleEntry& m)
    :
    moduleinfo_(m.moduleinfo_),
    ftpaddr_   (m.ftpaddr_)
  {}

  ModuleEntry(const char* module,
              const char* checksum,
              const char* ftpaddr),
    pdu_moduleinfo_(module, checksum),
    ftpaddr_       (ftpaddr)
  {}

  ModuleEntry& operator = (const ModuleEntry& rhs)
  {
    if (this != rhs)
    {
      moduleinfo_ = rhs.moduleinfo_;
      ftpaddr_ = rhs.ftpaddr_;
    }
    return *this;
  }
};

};

/*
 Read module information from 'sockfd',
 check that against server's current versions
 and send out downloadrequests for outdated
 modules.

 Returns:
    -1  on error
    0   client is up to date
    1   client will download new versions
*/

int read_client_versions(int sockfd, vector<pdu_versioninfo>& v)
{
  // XXX THIS WILL NOT WORK! The socket should be set
  // to blocking mode and read the data in a loop!
  for (int i = 0; i < numentries; i++)
  {
    memset(&vinfo, 0, sizeof(vinfo));
    nread = read(sockfd, &vinfo, sizeof(vinfo));

    if (nread < 0)
      return -1;

    v[i] = vinfo;
  }

  return i;
}

// For now, read the information from a separate file;
// NOTE: this creates the possibility for a serious error
// if the file and the real situation do not match.
/*
 * Version config file entries are of the form:
 *
 * module$checksum$ftpaddr$
 *
 * table.exe$asdinergoinwefoin$ftp://ns1.pokerspot.com/ftp/module.exe
 */
int read_server_version(vector<ModuleEntry>& v)
{
  // open & read file, add entries to 'v'
  char* module = NULL;
  char* ftpaddr = NULL;
  char* checksum = NULL;

  const int BufSize = 512;
  char readbuff[BufSize];	
  FILE* versionfd = fopen("client_versiondata.conf","ra");
  if (!versionfd)
  {
    fprintf(stderr, "Can't open version info file: clientversion.conf.\n");
    return -1;
  }

  while (fgets(readbuff, BufSize, versionfd))
  { 
    module = strtok(readbuff, "$");
    checksum = strtok(NULL, "$");
    ftpaddr = strtok(NULL, "$");

    if (ftpaddr != NULL)
    {
      v.push_back(ModuleEntry(module,
                              checksum,
                              ftpaddr));
    }
  }

  fclose(versionfd);
  return 0;
}


int send_download_requests(int sockfd,
                           const vector<ModuleEntry>& requests)
{
  return 0;
}


int process_moduleinfo(int sockfd)
{
  u_int16_t numentries = 0;

  // Read number of incoming version infos
  int nread = read(sockfd, &numentries, sizeof(numentries));
  if (nread < 0)
  {
    perror("process_moduleinfo: read numentries");
    return -1;
  }

  // Read incoming versioninfos from socket
  vector<pdu_versioninfo> client_versions(numentries);
  if (read_client_versions(sockfd, client_versions) < 0)
  {
    perror("process_moduleinfo: read_client_versions");
    return -1;
  }

  // Read current version info at server 
  vector<ModuleEntry> server_versions;
  if (read_server_versions(server_versions) < 0)
  {
    perror("process_moduleinfo: read_server_versions");
    return -1;
  }

  vector<ModuleEntry> download_requests;
  // For each server version entry, if client does
  // not have an identical moduleinfo, then add a
  // download request.
  for (int i = 0; i < server_versions.size(); i++)
  {
    ModuleEntry& e = server_versions[i];
    pdu_versioninfo vers(e.moduleinfo_.module_,
                         e.moduleinfo_.checksum_);

    // If the client does not have identical version
    // info, then add a download request
    vector<pdu_versioninfo>::const_iterator
      pos = client_versions.find(vers);

    if (pos == client_versions.end())
    {
      download_requests.push_back(e);
    }
  }

  return send_download_requests(sockfd, download_requests);
}

typedef struct pdu_downloadrequest
{
  u_int16_t numentries;
  // 'numentries' times this:
  u_byte_t module[16];
  u_int16_t ftpaddrlen;
  char* ftpaddress;
};

inline
bool operator == (const ModuleEntry& lhs, const VersionEntry& rhs)
{
  if (lhs.module.size() == 0)
    return false;
  else
    return lhs.module == rhs.module;
}


typedef vector<VersionEntry> Versions;

int check_client_version(int sockfd)
{
  Versions client_versions;
  Versions server_versions;
  Versions download_requests;

  // Read in the versions reported by client
  read_versions(sockfd, client_versions);

  // Read in the current versions from config file
  read_version_confid(server_versions);

  // Compare current versions against client's
  // versions, and add a download request if client
  // doesn't have an up-to-date version
  for (Versions::iterator i = server_versions.begin(),
                          e = server_versions.end();
       i != e;
       ++i)
  {
    Versions::iterator p = client_versions.find(*i);

    if (p == client_versions.end())
    { // Client doesn't have this module - add it
      download_requests.push_back(*i);
    }
    else if (p->version < i->version)
    { // Client's version is older - add it
      download_requests.push_back(*i);
    }
    else
    { // Client has an up-to-date version, ok
    }
  }
}


void PDUDownloadRequest::execute(GameState*)
{
}