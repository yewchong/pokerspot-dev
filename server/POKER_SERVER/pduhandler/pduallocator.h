#ifndef __table_pduallocator_h__
#define __table_pduallocator_h__

#include "../includes.h"
#include "../def.h"
#include <map>
USE_STD;

class Cpdu;

// This class handles the allocation of pdu objects
// For each (incoming) pdu type there is an PduAllocator
// subclass that allocates the particular pdu type.

struct PduAllocator
{
    virtual Cpdu* allocate() const = 0;
};


// Type definitions for the association map 
typedef map<u_int16_t, PduAllocator*, less<u_int16_t> > PduType2Allocator;
typedef PduType2Allocator::iterator PduType2AllocatorIT;
typedef PduType2Allocator::const_iterator PduType2AllocatorCIT;

//
// The following macro should be used to register
// incoming pdus so the poller will recognize it.
// As an example, the pduaction.cpp has the line:
// 
//    REGISTER_INCOMING_PDU(PDU_Play_Action, CpduAction);
//

#define REGISTER_INCOMING_PDU(pdu_type_id, pdu_type) struct Alloc_##pdu_type : public PduAllocator { Alloc_##pdu_type(u_int16_t type) { Cpdu::RegisterAllocator(type, this); } Cpdu* allocate() const { return new pdu_type(); } }; static Alloc_##pdu_type g_alloc_##pdu_type(pdu_type_id);


#endif
