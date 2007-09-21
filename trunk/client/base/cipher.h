#ifndef __base_cipher_h__
#define __base_cipher_h__

//
// Cipher a string
//

enum Cipher_Op
{
    CO_Decrypt = 0,
    CO_Encrypt = 1
};

extern CString do_cipher(const char* inbuf,
                         int buflen,
                         Cipher_Op operation);

#endif