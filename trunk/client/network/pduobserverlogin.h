#ifndef __table_pduobserverlogin_h__
#define __table_pduobserverlogin_h__

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSetupObserverLogin : public PDUHeader
{
  // Originated from client

  string_t username_;
  string_t password_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(string_t); }

  virtual u_byte_t* write(char* buffer) const
  {
    PDU_DEBUG("--- Send: Table Observer Login PDU ---");

    u_byte_t* pbyte = PDUHeader::write(buffer);

#ifdef PDU_DEBUG_
    CString s;
    CString n = MakeString(username_),
            p = MakeString(password_);
    s.Format("   username: %s password: %s", (LPCSTR)n, (LPCSTR)p);
    PDU_DEBUG((LPCSTR)s);
#endif

    // Write username & password
    memcpy(pbyte, username_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    memcpy(pbyte, password_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    return pbyte;
  }

  PDUSetupObserverLogin(const char* username,
                        const char* password)
    :
    PDUHeader(PDU_Setup_ObserverLogin, sizeOf(),
              username, password)
  {
    if (username != NULL && *username != '\0')
      strncpy(username_, username, PDU_STRINGSIZE);
    else
      memset(username_, 0, sizeof(username_));

    if (password != NULL && *password != '\0')
      strncpy(password_, password, PDU_STRINGSIZE);
    else
      memset(password_, 0, PDU_STRINGSIZE);
  }
};

END_NAMESPACE(Network)

#endif