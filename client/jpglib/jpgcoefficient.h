#ifndef CB__JPGCOEFFICIENT_H
#define CB__JPGCOEFFICIENT_H
//
// Copyright (c) 1997,1998 Colosseum Builders, Inc.
// All rights reserved.
//
// Colosseum Builders, Inc. makes no warranty, expressed or implied
// with regards to this software. It is provided as is.
//
// See the README.TXT file that came with this software for restrictions
// on the use and redistribution of this file or send E-mail to
// info@colosseumbuilders.com
//

//
//  Title:  JpegEncoderCoefficientBlock class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class is used to store DCT coefficients within component object.
//

#include "jpgpvt.h"

namespace ColosseumPrivate
{

typedef BYTE2 JpegCoefficientBlock [JPEGSAMPLESIZE] ;

}

#endif
