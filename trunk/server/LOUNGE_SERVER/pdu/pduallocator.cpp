/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : pduallocator.h
  Created  : 04/16/2000

  OVERVIEW : pdu type recognition & allocation.

 ****************************************************************************/

#include "pduallocator.h"
#include "pduunknown.h"
#include "pduchat.h"

#define DEFINE_INCOMING_PDU(pdu_type_id, pdu_type) \
struct Alloc_##pdu_type : public PduAllocator \
{ \
    Cpdu* allocate() const { return new pdu_type(); } \
}; \
\
static Alloc_##pdu_type g_alloc_##pdu_type;

DEFINE_INCOMING_PDU(PDU_Unknown, CpduUnknown);
DEFINE_INCOMING_PDU(PDU_Chat, CpduChat);

void RegisterAllocators()
{
    Cpdu::RegisterAllocator(PDU_Unknown, &g_alloc_CpduUnknown);
    Cpdu::RegisterAllocator(PDU_Chat, &g_alloc_CpduChat);
}
