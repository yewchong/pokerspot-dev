#ifndef __psi_ntwktype_h__
#define __psi_ntwktype_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : ntwktype.h
  Created  : 26.10.1999

  OVERVIEW : Data types used in Client/Server PDUs.

 ****************************************************************************/

typedef unsigned char  u_byte_t;
typedef unsigned short u_int16_t;
typedef unsigned int   u_int32_t;

#define PDU_STRINGSIZE 16
typedef char string_t[PDU_STRINGSIZE];
typedef char* message_t; 
#define PDU_MESSAGE_LEN 255

#endif