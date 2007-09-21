#ifndef __cwg_pduallocator_h__
#define __cwg_pduallocator_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : pduallocator.h
  Created  : 09/21/2001

  OVERVIEW : PDU type recognition & allocation.

 ****************************************************************************/


#include "common/compat.h"
#include "def.h"
#include <map>
USING_NS_T(std, map);

class CPDU;

// This class handles the allocation of PDU objects
// For each (incoming) PDU type there is an PDUAllocator
// subclass that allocates the particular PDU type.

struct PDUAllocator
{
  virtual CPDU* allocate() const = 0;
};


// Type definitions for the association map 
typedef map<u_int16_t, PDUAllocator*> PDUType2Allocator;
typedef PDUType2Allocator::iterator PDUType2AllocatorIT;
typedef PDUType2Allocator::const_iterator PDUType2AllocatorCIT;

//
// The following macro should be used to register
// incoming pdus so the poller will recognize it.
// As an example, the pduaction.cpp has the line:
// 
//    REGISTER_INCOMING_PDU(PDU_Play_Action, CPDUAction);
//

#define REGISTER_INCOMING_PDU(pdu_type_id, pdu_type) \
struct Alloc_##pdu_type : public PDUAllocator \
{ \
    Alloc_##pdu_type(u_int16_t type) \
    { \
        CPDU::RegisterAllocator(type, this); \
    } \
    \
    CPDU* allocate() const { return new pdu_type(); } \
}; \
\
static Alloc_##pdu_type g_alloc_##pdu_type(pdu_type_id);

#endif