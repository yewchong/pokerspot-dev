
// Non-blocking io read is complicated.
Cpdu* CPlayer::readData()
{
    if (currentPdu_)
    {
        int err = currentPdu_->readData(connfd_);
        if (currentPdu_->finished())
        {
            Cpdu* pdu = currentPdu_;
            currentPdu_ = NULL;
            return pdu;
        }
    }
    else
    {
        int pdu_type = Cpdu::ReadData(connfd_);
        if (pdu_type != 0)
        {
        }
    }
}


  // Read all available bytes
  char* bptr = &buf[ofs_];
  int n = 0, buflen = PDU_HEADER_SIZE;

  //
  // Read the PDU header
  //
  while (buflen > 0 &&
         (n = PSI_SSL_Read(s, bptr, buflen)) != SOCKET_ERROR &&
          n > 0)
  {
    bptr += n;
    buflen -= n;
  }
