#ifndef __psi_nbiobuf_h__
#define __psi_nbiobuf_h__


class Cpdu;


class CnbIoBuf
{
public:
    int readPdu(int sockfd, Cpdu** ppPdu);
    Cpdu* makePdu(const char*);

private:
    // Incoming PDU is read into this buffer as
    // it arrives. When it is complete, the pdu
    // is constructed out of the buffer and interpreted.
    char incoming_[MAXBUFFERSIZE];
    int  ofs_;
};



#endif