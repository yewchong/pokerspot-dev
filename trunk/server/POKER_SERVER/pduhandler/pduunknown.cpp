#include "pduunknown.h"
#include "../sys.h"

REGISTER_INCOMING_PDU(PDU_Unknown, CpduUnknown);

//
// This is a dummy pdu type used when the
// incoming pdu type is not recognized. The
// 'grok' member function just prints out
// an error message.
//

CpduUnknown::CpduUnknown()
{}

void CpduUnknown::grok(SOCKET, CTable*)
{
/*
    char buf[128];
    sprintf(buf, "Ignoring unknown pdu type: %d, length: %d",
            type_, length_);
    Sys_LogError(buf);
*/

    delete this;
}