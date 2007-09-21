#ifndef __table_def_h__
#define __table_def_h__

#define NUMOF(array) (sizeof(array) / sizeof(array[0]))

#define MAXBUFFERSIZE		1024 // Maximum PDU size!

#ifdef _WIN32
    #define USE_STD using namespace std;
    // Disable harmless STL debug symbol warnings
    #pragma warning(disable : 4786)
#else
    #define USE_STD
#endif

// 20-second timeout for login handshake
#define CWG_TABLEHANDSHAKE_TIMEOUT 20000

#endif