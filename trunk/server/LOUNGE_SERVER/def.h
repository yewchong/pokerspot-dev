#ifndef __lounge_def_h__
#define __lounge_def_h__

#define NUMOF(array) (sizeof(array) / sizeof(array[0]))

#ifdef _WIN32
    #define USE_STD using namespace std;
    // Disable harmless STL debug symbol warnings
    #pragma warning(disable : 4786)
#else
    #define USE_STD
#endif

#define PSPOT_LOUNGESERVER_PORT 8500
//#define PSPOT_LOUNGESERVER_PORT 8100 // for private testing

#define MAX_LOGIN_RETRIES 2
#define LOGIN_TIMEOUT 30


#define CWG_LOADBALANCER_IP   16777343 // local host: 127.0.0.1
#define CWG_LOADBALANCER_PORT 8300

#define CWG_HANDSHAKE_TIMEOUT 15000    // 15 second timeout for handshakes

#endif
