/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : Class Cpdu
  Created  : 04/16/2000

  OVERVIEW : PDU base class.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "pdu/pduunknown.h"
#include "lounge/lounge.h"
#include "loadbalancer/loadbalancer.h"
#include "sys.h"
#include "def.h"
#include <memory>

Cpdu::Cpdu()
  :
	type_  (0),
	length_(0)
{
	memset(value_, 0, sizeof(value_));
}


Cpdu::Cpdu(int type, int length, const char* data)
  :
  type_  (type),
  length_(length)
{
  if (length >= MAXBUFFERSIZE)
  {
    Sys_LogError("Cpdu::Cpdu: length >= MAXBUFFERSIZE");
    length_ = PDU_HEADERSIZE;
  }
  else
  {
    memcpy(value_, data, length);
  }
}


Cpdu::~Cpdu()
{}


int Cpdu::sendTo(SOCKET s)
{
  // bot_X bots and player proxies have INVALID_SOCKET
  if (s == -1 || s == INVALID_SOCKET)
    return 0;

	return Sys_send(s, value_, length_);
};


int Cpdu::sendTo(CPlayer* player)
{
  return player->send(*this);
}

int Cpdu::sendTo(CTable* table)
{
  return table->send(*this);
}

// Broadcast this pdu to all players.
int Cpdu::broadcastToPlayers(CPlayer* except)
{
    int rc = 0;

    for (PlayerIterator p = CLounge::Inst()->allPlayers();
         p.more();
         p.next())
    {
        CPlayer* player = p.current();
        if (player != except)
        {
            if (!player->getNetworkError())
            {
                // Testing: Don't send to 'bot_X' players
                if (player->getSocket() == -1)
                {
                    continue;
                }

                int rc = sendTo(player->getSocket());                
                if (rc == -1)
                {
                    char s[200];
                    sprintf(s, "Cpdu::broadcast: Player %s connection has died.", player->getUsername());
                    Sys_LogError(s);
                    player->setNetworkError(true);
                }
            }
        }
    }

    return rc;
};

void Cpdu::stuffHeader(void)
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
int Cpdu::Peek(SOCKET sd)
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
#ifdef _WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {   // ok, data has not arrived yet -
            // next iteration will get it
        }
        else
        {   // connection has died - return error
            pdu_type = -1;
        }
#else
        if (errno == EAGAIN)
        {   // ok, data has not arrived yet -
            // next iteration will get it
            char buf[100];
            sprintf(buf, "Peek: %i, will retry", err);
            Sys_LogError(buf);
        }
        else
        {   // connection has died - return error
            extern int ssl_check_nbio(int, int);
            ssl_check_nbio(sd, err);

            pdu_type = -1;
        }
#endif
    }

    return pdu_type;
}


Cpdu* Cpdu::Read(SOCKET sd, u_int16_t pdu_type)
{
	Cpdu* pdu = AllocPdu(pdu_type);

  if (pdu)
	{
		int rc = pdu->readData(sd);

		if (rc == -1)
		{	// read failed! clean up
			delete pdu;
			pdu = NULL;
			Sys_LogError("CPdu::Read: pdu->readData");
		}
	}

	return pdu;
}


void Cpdu::grok(SOCKET sd, CPlayer*)
{
  // The PDU deallocates itself after it is done
  delete this;
}


void Cpdu::grok(SOCKET, CTable*)
{
  // The PDU deallocates itself after it is done
  delete this;
}


void Cpdu::grok(SOCKET sd, CLoadBalancer*)
{
  // The PDU deallocates itself after it is done
  delete this;
}


int Cpdu::readData(SOCKET sd)
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


int Cpdu::writeData(SOCKET sd)
{
    return Sys_send(sd, value_, length_);
}


// Global map maintaining <pdu type id, PduAllocator>
// pairs.
PduType2Allocator* g_alloc_map = NULL;

void Cpdu::RegisterAllocator(u_int16_t type_id, PduAllocator* alloc)
{
    if (!g_alloc_map)
    {
        g_alloc_map = new PduType2Allocator();
        ASSERT(g_alloc_map);
        static auto_ptr<PduType2Allocator> garbage_collector(g_alloc_map);
    }
    g_alloc_map->insert(PduType2Allocator::value_type(type_id, alloc));
}


Cpdu* Cpdu::AllocPdu(u_int16_t pdu_type)
{
    Cpdu* pdu = NULL;

    // Look up the allocator for this pdu type
    PduType2AllocatorIT it = g_alloc_map->find(pdu_type);

    if (it != g_alloc_map->end())
    {   // Allocator found, use it to instantiate new pdu
        pdu = (*it).second->allocate();
    }
    else
    {   // If pdu type is not recognized create an
        // 'unknown' pdu. 
        printf("PDU type not recognized: %d\n", pdu_type);
        pdu = new CpduUnknown();
    }

    if (!pdu)
    {
        Sys_LogError("Cpdu::AllocPdu: out of memory");
    }

    return pdu;
}


int Cpdu::sendToLoadBalancer()
{
  int rc = 0;

  if (CLoadBalancer::Inst())
    rc = sendTo(CLoadBalancer::Inst()->getSocket());

  return rc;
}


int Cpdu::sendToLounges()
{
  return 0;
}


int Cpdu::setData(u_int16_t type,
                  u_int16_t length,
                  const char* data)
{
  if (length >= MAXBUFFERSIZE)
  {
    Sys_LogError("Cpdu::setData: length >= MAXBUFFERSIZE");
    length_ = PDU_HEADERSIZE;
    return -1;
  }
  else
  {
    type_ = type;
    length_ = length;
    memcpy(value_, data, length);
    return length_;
  }
}