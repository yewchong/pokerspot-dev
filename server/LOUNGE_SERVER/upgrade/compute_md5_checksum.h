#ifndef __psi_compute_md5_checksum_h__
#define __psi_compute_md5_checksum_h__

#include "psi_win32.h"

extern
int compute_md5_checksum(const char* filename,
                         u_byte_t* outdigest);

#endif
