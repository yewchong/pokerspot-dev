#ifndef __cwg_networkalign_h__
#define __cwg_networkalign_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : networkalign.h
  Created  : 10/01/2001

  OVERVIEW : PDU structures that are to be sent over network
             and aligned properly should be wrapped inside these
             macros. They ensure the structure is aligned on two-byte
             boundaries. This is needed so that the client and the
             server agree on structure alignment.

 ****************************************************************************/


// These macros will align structures sent over
// network on two byte boundaries.
#define NETWORK_ALIGN_BEGIN pack (push, 2)
#define NETWORK_ALIGN_END pack (pop)


#endif