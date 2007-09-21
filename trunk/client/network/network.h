#ifndef __psi_network_h__
#define __psi_network_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : network.h
  Created  : 26.10.1999

  OVERVIEW : Client/Server Protocol Data Unit definitions.

 ****************************************************************************/

#include "network/ntwktype.h"
#include "common/pdutypes.h"
#include "network/networkbuffer.h"
#include "network/compat.h"
#include <assert.h>
#include <string>
USING_NS_T(std, string);
//
// PDU Debugging helpers.
// Application must define PDUDebugPrint and
// SysDebugPrint functions.
//
#ifdef DEBUG_CHAT_MESSAGES_
extern void PDUDebugPrint(const char* msg, DWORD flags);
extern void SysDebugPrint(const char* msg, DWORD flags);
#define PDU_DEBUG(x) PDUDebugPrint(x, 0)
#define PDU_ASSERT(x) \
  { if (!(x)) { \
      std::string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = std::string("*** PDU Assertion Failed: expression: '") + std::string(#x) + \
          std::string("' file: ") + std::string(__FILE__) + std::string(" line: ") + \
          std::string(buf); \
      PDUDebugPrint(s.c_str(), 2); \
    } \
  }
#define SYSMSG_ASSERT(x) \
  { if (!(x)) { \
      std::string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = std::string("*** Pot Assertion Failed: expression: '") + std::string(#x) + \
          std::string("' file: ") + std::string(__FILE__) + std::string(" line: ") + \
          std::string(buf); \
      SysDebugPrint(s.c_str(), 2); \
    } \
  }
#else
#define PDU_DEBUG(x) { OutputDebugString(x); OutputDebugString("\n"); } 
#define PDU_ASSERT(x) \
  { if (!(x)) { \
      std::string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = std::string("*** PDU Assertion Failed: expression: '") + std::string(#x) + \
          std::string("' file: ") + std::string(__FILE__) + std::string(" line: ") + \
          std::string(buf); \
      PDU_DEBUG(s.c_str()); \
      assert(s.c_str() == 0); \
    } \
  }
#define SYSMSG_ASSERT(x)
#endif

#ifdef _WIN32
  namespace Base { class GameState; }
  using Base::GameState;
#else
  class GameState;
#endif  

#define PSPOT_LOADBALANCER_HOST "127.0.0.1"
#define PSPOT_LOADBALANCER_PORT 8301
#define PSPOT_PLAYMONEY_HOST "127.0.0.1"
#define PSPOT_PLAYMONEY_PORT 8500
#define PSPOT_TABLESERVER_PORT 8502

#define CLIENT_CHECKSUM_SIZE 16

BEGIN_NAMESPACE(Network)

/*
 * PDU header shared by all PDUs.
 */

//
// NOTE: WILL USE NEW PDU HEADER WITHOUT
// USERNAME AND PASSWORD
//


#define PDU_OFS_TYPE 0
#define PDU_OFS_LENGTH 2
#ifdef OLD_PDUHEADER_W_USERNAMEANDPASSWORD
  #define PDU_OFS_USERNAME 4
  #define PDU_OFS_PASSWORD (PDU_OFS_USERNAME + PDU_STRINGSIZE)
  #define PDU_HEADER_SIZE (2 * sizeof(u_int16_t) + 2 * PDU_STRINGSIZE)
#else
   #define PDU_HEADER_SIZE (2 * sizeof(u_int16_t))
#endif
// 64 kB should be enough for pdu's
#define PDU_MAX_SIZE (1024 * 64)

//
// New pdu header without
// username and password.
//
struct PDUHeader
{
#ifdef _DEBUG
  static DWORD AllocBalance_;
#endif

  u_int16_t type_;   
  u_int16_t length_;

  // Client side PDU action: 
  virtual void execute(GameState*) {};

  // PDU's compute their size explicitely
  virtual size_t sizeOf() const { return 2 * sizeof(u_int16_t); }

  // Serialize PDU data to a char buffer
  virtual u_byte_t* write(char* buffer) const
  {
    u_int16_t* pint = (u_int16_t*)buffer;
    *pint++ = htons(type_);
    *pint++ = htons(length_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   type: %d length: %d", type_, length_);
    PDU_DEBUG((LPCSTR)s);
#endif
    return (u_byte_t*)pint;
  }

  virtual void serialize(WriteBuffer& wbuf) const
  {
    wbuf << type_;
    wbuf << length_;
  }

  PDUHeader(u_int16_t type,
            u_int16_t length,
            const char* dummy1,
            const char* dummy2)
    : type_(type), length_(length)
  {
#ifdef _DEBUG      
    ++AllocBalance_;
#endif
  }

  PDUHeader(const PDUHeader& p)
    : type_(p.type_), length_(p.length_)
  {
#ifdef _DEBUG      
    ++AllocBalance_;
#endif
  }

  PDUHeader(const char* buf)
    :
    type_(ntohs(*((u_int16_t*)&buf[PDU_OFS_TYPE]))),
    length_(ntohs(*((u_int16_t*)&buf[PDU_OFS_LENGTH])))
  {
#ifdef _DEBUG      
    ++AllocBalance_;
#endif
  }

  PDUHeader(ReadBuffer& rbuf)
    : type_(0), length_(0)
  {
#ifdef _DEBUG      
    ++AllocBalance_;
#endif
    rbuf >> type_;
    rbuf >> length_;
  }
  
  virtual ~PDUHeader()
  {
#ifdef _DEBUG
    --AllocBalance_;
#endif
  }

  virtual void checkValid()
  {
    PDU_ASSERT(length_ >= PDU_HEADER_SIZE);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   type: %d length: %d", type_, length_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};

inline
bool IsPDU(PDUHeader* pduh, PDU_Type t)
{
  return (pduh->type_ == t);
}

// NOTE: The PDU string is not necessarily 0-terminated!!!

// Make a CString out of a PDU string
inline CString MakeString(const string_t pdustr)
{
  char buf[PDU_STRINGSIZE+1];
  memset(buf, 0, sizeof(buf));
  strncpy(buf, pdustr, PDU_STRINGSIZE);
  return CString(buf);
}

typedef PDUHeader* (*PDUAllocFunc)(const char* buf);

struct ThreadInfo
{
  SOCKET s;
  HWND   hWnd;
  BOOL   bQuit;
  BOOL   bRunning;
};

extern UINT SocketReaderProc(VOID* pV);
extern int  SendPDU         (SOCKET s, const PDUHeader& pdu);
extern int  Send            (SOCKET s, const char* buf, size_t nbytes);
extern int  ReadPDU         (SOCKET s, PDUHeader** ppPDU);

END_NAMESPACE(Network)



#endif