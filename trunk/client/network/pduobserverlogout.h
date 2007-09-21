#ifndef __table_pduobserverlogout_h__
#define __table_pduobserverlogout_h__

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSetupObserverLogout : public PDUHeader
{
  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
    PDU_DEBUG("--- Send: Table Observer Logout PDU ---");
    u_byte_t* p = PDUHeader::write(buffer);
    return p;
  }

  PDUSetupObserverLogout(const char* username,
                         const char* password)
    :
    PDUHeader(PDU_Setup_ObserverLogout,
              sizeOf(), username, password)
  {}
};


END_NAMESPACE(Network)

#endif