#include "def.h"
#include "sys.h"
#include "pduhandler.h"
#include "pduunknown.h"
#include "table/table.h"
#include "ring/ring.h"
#include "poller/poller.h"


Cpdu::Cpdu()
    :
	type_  (0),
	length_(0)
{
	memset(value_, 0, sizeof(value_));
}

Cpdu::~Cpdu()
{}

int Cpdu::getHeader(SOCKET s)
{
	pduh_t *pPdu;
	int rval;

	pPdu = (pduh_t *)value_;

	rval = Sys_recv(s, value_, PDU_HEADERSIZE);

	if (rval == PDU_HEADERSIZE)
	{
		type_ = ntohs(pPdu->type);
		length_ = ntohs(pPdu->length);
	}

	return rval;
};

int Cpdu::getData(SOCKET s)
{

	return Sys_recv(s, (value_ + PDU_HEADERSIZE), getLength() - PDU_HEADERSIZE);
};

int Cpdu::sendTo(SOCKET s)
{
	int retval = 0;

    if (s != -1)
	    retval = Sys_send(s, value_, length_);

	return retval;
};

// Broadcast this pdu to all players in 'ring'.
// Player 'except' is skipped.
// Added code on 4/13/00 to send ONLY to players that are PLAYER_STATE_SITTING,
// this keeps us from having to do game_catchup() functions later on, so that the client
// can merely display a "Current Hand In Progress -- Please Wait" message.  Works
// beautifully.
int Cpdu::broadcastTo(Cring* ring, CPlayer* except)
{
    int rc = 0;

    SingleLock l(&CTable::Instance()->getTableMutex());
    if (!l.lock()) return -1;

    CRingIterator it(ring);
    if (!it.current()) return 0;

    do
    {
        CPlayer* player = it.current()->getPlayer();
        if (player && (player != except) &&
            // XXX: let newly logged in player see pdus too!
            /*(!player->matchState(PLAYER_STATE_BOUGHTIN)) &&*/
             !player->matchState(PLAYER_STATE_LOGGINGOUT))
        {
            rc = sendTo(player->getConnfd());

            if (rc == -1)
            {
                player->setSendError(true);
            }
        }
    }
    while (++it);        

    // Send also to observers
    CTable* table = CTable::Instance();
    for (int i = 0; i < CTable::MaxObservers; i++)
    {
        CPlayer* observer = table->getObserver(i);
        if (observer && (observer->getConnfd() != -1))
        {
            rc = sendTo(observer->getConnfd());

            if (rc == -1)
            {
                observer->setSendError(true);
            }
        }
    }

    return rc;
};

void Cpdu::stuffHeader(void)
{
	pduh_t *pduHeader = (pduh_t *)value_;

	pduHeader->type = htons(type_);
	pduHeader->length = htons(length_);
};

bool Cpdu::peekHeader(SOCKET s)
{
	if (Sys_peek(s, value_, PDU_HEADERSIZE) == PDU_HEADERSIZE)
		return true;
	else
		return false;
};

bool Cpdu::peekData(SOCKET s, size_t length)
{
	char *pValue = value_ + PDU_HEADERSIZE;

	if (Sys_peek(s, pValue, length) == (int)length)
		return true;
	else
		return false;
	
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
    int n = 0;

    char buffer[MAXBUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = 0;

    bytesRead = Sys_peek(sd, buffer, PDU_HEADER_SIZE);
    if (bytesRead == PDU_HEADER_SIZE)
    {
		pduh_t* header = (pduh_t*)buffer;
		u_int16_t length = ntohs(header->length);

        if (length > PDU_HEADER_SIZE)
        {
            if ((n = Sys_peek(sd, buffer, length)) == length)
            {
                pdu_type = ntohs(header->type);
            }
            else
            {
              static int c = 30;
              if (c > 0)
              {
                c--;
                char buf[200];
                sprintf(buf, "Network error(3) socket %d, length %i read %i\n", sd, length, n);
                Sys_LogError(buf);
              }
            }
        }
        else
        {
            pdu_type = ntohs(header->type);
        }
    }
    else
    {   // detect broken connection
        extern int ssl_check_nbio(int, int);
        ssl_check_nbio(sd, bytesRead);

#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {   // ok, data has not arrived yet -
            // next iteration will get it
            Sys_LogError("Would block - will retry!");
        }
        else
        {   // connection has died - return error
            Sys_LogError("Error, connection will be closed.");
            pdu_type = -1;
        }
#else
        if (errno == EAGAIN)
        {   // ok, data has not arrived yet -
            // next iteration will get it
            static int c = 30;
            if (c > 0)
            {
              c--;
              char buf[200];
              sprintf(buf, "Network error(EAGAIN) socket %d\n", sd);
              Sys_LogError(buf);
            }
        }
        else
        {   // connection has died - return error
            pdu_type = -1;

	    static int c = 30;
	    if (c > 0)
	      {
		c--;
		char buf[200];
		sprintf(buf, "Network error(2) socket %d\n", sd);
		Sys_LogError(buf);
	      }
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

//
// The default handling for a pdu is to place
// it in the player's queue. If a pdu is handled
// differently, it must override this mfunction.
//
void Cpdu::grok(SOCKET sd, CTable* table)
{
    CPlayer* player = table->getPlayerRawSocket(sd);
    if (player)
    {
        bool rc = player->queuePdu(this);
        if (!rc)
        {
            // Queueing the pdu failed!
            // This means the system is hosed, but to
            // prevent further memory leaks, clean up
            char buf[128];
            sprintf(buf, "Cpdu::grok: failed to queue pdu for socket %d!", sd);
            Sys_LogError(buf);
            delete this;
        }
    }
    else
    {
        char buf[128];
        sprintf(buf, "Cpdu::grok: player for socket %d not found!", sd);
        Sys_LogError(buf);
        delete this;
    }
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

int Cpdu::readData(const char* buf)
{
    pduh_t* header = (pduh_t*)buf;

    type_ = ntohs(header->type);
	length_ = ntohs(header->length);
    memcpy(value_, buf, length_);
    return length_;
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
        // it makes no sense to continue if we're out
        // of memory already at start-up!
        ASSERT(g_alloc_map);
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
    {   
        printf("PDU type not recognized: %d\n", pdu_type);
        pdu = new CpduUnknown();
    }

    if (!pdu)
    {
        Sys_LogError("Cpdu::AllocPdu: out of memory");
    }

    return pdu;
}

u_int16_t Cpdu::getSequenceNumber() const
{
    // By default, pdus are not sequenced; 
    // those subclasses that are to be queued in player's
    // queue have to override this
    return PDU_NOT_SEQUENCED;
}
