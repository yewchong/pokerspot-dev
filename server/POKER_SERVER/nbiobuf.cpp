This goes to poller:
/*
                CnbIoBuf* iobuf = client_[i].iobuf_;
                ASSERT(iobuf);

                if (iobuf)
                {
                    Cpdu* pdu = NULL;
                    if (iobuf->readPdu(sockfd, &pdu) < 0)
                    {
                        pTable_->disconnectPlayer(sockfd);
                    }
                    else if (pdu)
                    {
                        // The pdu interprets itself and
                        // deallocates if necessary
                        pdu->grok(sockfd, pTable_);
                        pdu = NULL;
                    }

                    FD_CLR((unsigned)sockfd, &rset);  
                }
                else
                {   
                    int pdu_type = Cpdu::Peek(sockfd);
                    if (pdu_type == -1)
                    {
                        // this connection has died 
                        printf("Error reading socket %d - will terminate connection.\n", sockfd);
                        pTable_->disconnectPlayer(sockfd);
                    }
                    else if (pdu_type != 0)
                    {
                        //
                        // the whole pdu has arrived - process it
                        //
                        Cpdu* pdu = Cpdu::Read(sockfd, pdu_type);
    
                        if (pdu)
                        {
                            pdu->grok(sockfd, pTable_);
                            pdu = NULL;
                            FD_CLR((unsigned)sockfd, &rset);
                        }
                    }
                    else
                    {
                        // the whole pdu has not arrived yet - leave
                        // it to the next iteration
                    }   
                }
*/


// This member function reads bytes available in socket.
// The data is stored to a buffer; if the whole pdu has
// arrived, the pdu is created and returned. Otherwise
// the data is left in the buffer waiting for the whole
// pdu to arrive later.
int CnbIoBuf::readPdu(int sockfd, Cpdu** ppPDU)
{
    if (ofs_ < PDU_HEADER_SIZE)
    {
        // First read the header
        int nread = Sys_recv(sockfd, &incoming_[ofs_], PDU_HEADER_SIZE - ofs_);

        if (nread == -1)
        {
            if (Sys_GetLastError() == EAGAIN)
                return ofs_; // wait until more data arrives
            else
                return -1; // error occurred
        }

        ofs_ += nread;
    }

    if (ofs_ >= PDU_HEADER_SIZE)
    {
        // Then read extra bytes if any
        pduh_t* header = (pduh_t*)incoming_;
        int length = ntohs(header->length);
        if (ofs_ < length)
        {
            int nread = Sys_recv(sockfd, &incoming_[ofs_], length - PDU_HEADER_SIZE);

            if (nread == -1)
            {
                if (Sys_GetLastError() == EAGAIN)
                    return ofs_; // wait until more data arrives
                else
                    return -1; // error occurred
            }

            ofs_ += nread;
        }

        // Do we have the complete pdu?
        if (ofs_ == length)
        {
            Cpdu* pdu = makePdu(incoming_);
            ofs_ = 0;
            memset(incoming_, 0, sizeof(incoming_));
            *ppPDU = pdu;
        }
    }

    return ofs_;
}

Cpdu* CnbIoBuf::makePdu(const char* buf)
{
    pduh_t* header = (pduh_t*)buf;
    u_int16_t pdu_type = ntohs(header->type);
    Cpdu* pdu = Cpdu::AllocPdu(pdu_type);
    if (pdu)
    {
        pdu->readData(buf);
    }
    return pdu;
}