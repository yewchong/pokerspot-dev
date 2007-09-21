#include "stdafx.h"
#include "cipher.h"
#include "network/compat.h"
#include <openssl/evp.h>


const int C1 = 52845,
          C2 = 22719;

CString encrypt(const CString& s, WORD key)
{
    CString result(s);
    
    for (int i = 0; i < s.GetLength(); i++)
    {
        result.SetAt(i, char(byte(s[i]) ^ (key >> 8)));
        key = (byte(result[i]) + key) * C1 + C2;
    }

    //result.SetAt(i, '\0');

    return result;
}

CString decrypt(const CString& s, WORD key)
{
    CString result(s);

    for (int i = 0; i < s.GetLength(); i++)
    {
        result.SetAt(i, char(byte(s[i]) ^ (key >> 8)));
        key = (byte(s[i]) + key) * C1 + C2;
    }

    //result.SetAt(i, '\0');

    return result;
}

CString do_cipher(const char* inbuf,
                  int buflen,
                  Cipher_Op op)
{
    if (op == CO_Encrypt)
    {
        return encrypt(inbuf, 8932+1);        
    }
    else
    {
        return decrypt(inbuf, 8932+1);
    }
}

