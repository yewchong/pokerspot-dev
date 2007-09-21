#ifdef _WIN32
  // Link sockets import library
  #pragma comment (lib, "wsock32.lib")
  // Link SLL libraries
/*#ifdef _DEBUG
  #pragma comment(lib, "c:\\openssl-0.9.4\\bin32\\ssleay32d.lib")
  #pragma comment(lib, "c:\\openssl-0.9.4\\bin32\\libeay32d.lib")
#else*/
  #pragma comment(lib, "c:\\openssl-0.9.5a\\out32dll\\ssleay32.lib")
  #pragma comment(lib, "c:\\openssl-0.9.5a\\out32dll\\libeay32.lib")
//#endif
  extern int InitWinSock();
  extern int ShutdownWinSock();
  extern void CtrlPressed(int);

int SSLON = 0;
int SSLON_2 = 0;


void CtrlPressed(int i)
{
    ShutdownWinSock();
}

#endif