/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPdu
  Created  : 09/21/2001

  OVERVIEW : PDU (Protocol Data Unit) base class.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "pdu/pduunknown.h"
#include "loadbalancer/loadbalancer.h"
#include "common/sys.h"
#include "common/errorlog.h"
#include <memory>
#include <assert.h>
USING_NS_T(std, auto_ptr);


CPDU::CPDU()
  :
	type_  (0),
	length_(0)
{
	memset(value_, 0, sizeof(value_));
}

CPDU::~CPDU()
{}

int CPDU::sendTo(SOCKET s)
{
  // Bots have socket -1
  if (s == -1)
    return 0;

	return Sys_send(s, value_, length_);
};

// Broadcast this pdu to all players in 'container'.
int CPDU::broadcast(SOCKET except_this_sd)
{
  int rc = 0;
  
  for (LoungeIterator li = CLB::Inst()->getLounges();
       li.more();
       li.next())
  {
    CLounge* lounge = li.current();
    if (lounge && lounge->getSocket() != except_this_sd)
    {
      if (!lounge->getNetworkError())
      {        
        rc = sendTo(lounge->getSocket());                
        if (rc == -1)
        {
          Err::Log() << "CPDU::broadcast: Lounge connection ("
                     << lounge->getSocket() << ") has died.\n";
          lounge->setNetworkError(true);
        }
      }
    }
  }
  
  return rc;
};

void CPDU::stuffHeader()
{
	pduh_t* pduHeader = (pduh_t*)value_;

	pduHeader->type = htons(type_);
	pduHeader->length = htons(length_);
};

//
// This mfunction peeks into the socket and
// if the whole pdu has arrived, it returns
// the type of the incoming pdu (> 0).
// If the pdu has not arrived, returns 0.
// If an error occurs, returns -1.
//
int CPDU::Peek(SOCKET sd)
{
  int pdu_type = 0;
  
  char buffer[MAXBUFFERSIZE];
  memset(buffer, 0, sizeof(buffer));
  
  int err = Sys_peek(sd, buffer, PDU_HEADER_SIZE);
  if (err == PDU_HEADER_SIZE)
  {
    pduh_t* header = (pduh_t*)buffer;
    u_int16_t length = ntohs(header->length);
    
    if (length > PDU_HEADER_SIZE)
    {
      if (Sys_peek(sd, buffer, length) == length)
      {
        pdu_type = ntohs(header->type);
      }
    }
    else
    {
      pdu_type = ntohs(header->type);
    }
  }
  else
  {   // detect broken connection
    if (Sys_ErrIsWouldBlock())
    { // ok, data has not arrived yet -
      // next iteration will get it
    }
    else
    {   // connection has died - return error
      pdu_type = -1;
    }
 }
  
  return pdu_type;
}


CPDU* CPDU::Read(SOCKET sd, u_int16_t pdu_type)
{
	CPDU* pdu = AllocPDU(pdu_type);

  if (pdu)
	{
		int rc = pdu->readData(sd);

		if (rc == -1)
		{	// read failed! clean up
			delete pdu;
			pdu = NULL;
      Err:: Log() << "CPdu::Read: pdu->readData\n";
		}
	}

	return pdu;
}


void CPDU::grok(SOCKET, CLounge*)
{
    // The PDU destroys itself after it is done
    delete this;
}


int CPDU::readData(SOCKET sd)
{
	// Read the header
	int rc = Sys_recv(sd, value_, sizeof(pduh_t));

	if (rc != -1)
	{
		pduh_t* header = (pduh_t*)value_;

		type_ = ntohs(header->type);
		length_ = ntohs(header->length);

		// Read rest of the data
		if (length_ > PDU_HEADER_SIZE)
		{
			rc = Sys_recv(sd, value_ + PDU_HEADER_SIZE, length_ - PDU_HEADER_SIZE);
		}
	}
	return rc;
}


int CPDU::writeData(SOCKET sd)
{
    return Sys_send(sd, value_, length_);
}


// Global map maintaining <pdu type id, PduAllocator>
// pairs.
PDUType2Allocator* g_alloc_map = NULL;

void CPDU::RegisterAllocator(u_int16_t type_id, PDUAllocator* alloc)
{
  if (!g_alloc_map)
  {
    g_alloc_map = new PDUType2Allocator();
    assert(g_alloc_map);
    static auto_ptr<PDUType2Allocator> garbage_collector(g_alloc_map);
  }
  g_alloc_map->insert(PDUType2Allocator::value_type(type_id, alloc));
}


CPDU* CPDU::AllocPDU(u_int16_t pdu_type)
{
  CPDU* pdu = NULL;
  
  // Look up the allocator for this pdu type
  PDUType2AllocatorIT it = g_alloc_map->find(pdu_type);
  
  if (it != g_alloc_map->end())
  {   // Allocator found, use it to instantiate new pdu
    pdu = (*it).second->allocate();
  }
  else
  {   // If pdu type is not recognized create an
    // 'unknown' pdu. 
    pdu = new CpduUnknown();
  }
  
  if (!pdu)
  {
    Err::Log() << "CPDU::AllocPdu: out of memory.\n";
  }
  
  return pdu;
}