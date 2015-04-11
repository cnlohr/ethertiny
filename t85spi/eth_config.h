#ifndef _ETH_CONFIG_H
#define _ETH_CONFIG_H

//Terrifying though it may be... This is how it's all configured.

//If we are using an external driver, we must connect it this way:
//RO -> PB0
//DI <- PB1
//DE <- PB2
//#define USES_EXTERNAL_DRIVER

//Warning: YOU Must use with an ATTinyX5 series chip or another chip
//that allows you to tune OSCCAL to a high enough frequency.

//On the ATTiny85, you have 512bytes of RAM.  
// The stack uses ETBUFFERSIZE + 16 (Manchester table) + 8 (internal states) + 6(MAC, but not in core)
// Ram Budget:
//   ETBUFFERSIZE                  (n)
//   Various internal MAC states:   6
//   IP Stack:                     36
//   Manchester table:             16
//    (Requries ~24 bytes free stack)
//(We need to keep the manchester in RAM)
//For other type-size reasons, you cannot exceed a buffer size of 512.  (Problems with low-level ASM code)

//Warning: When receiving packets, you will only be able to receive packets
// less than 1/3 the size of preamble + etbuffersize.

#define ETBUFFERSIZE (360)
#define MAX_FRAMELEN (ETBUFFERSIZE)
#define TX_SCRATCHES 1
#define RX_BUFFER_START 0
#define RX_BUFFER_END   MAX_FRAMELEN


//OSC20 will depend on the specific chip.  Needs to be AS CLOSE TO 20 MHZ AS POSSIBLE!
//TUNE THIS!
#define OSC20 0xbb

//OSCHIGH needs to be as fast as the chip goes, but less than ~35 MHz. IT MUST EXCEED 30 MHZ!
#define OSCHIGH 0xFF  //~31.5 MHz.

#define INCLUDE_UDP


//Turn off the DDR when the port is not in use.
//Saves power, seems to work better?
#define SMARTDDR

//Extend the NLP by ~40ns
//#define LONGNLP

//Flip the NLP upside down.
//#define INVERTNLP

//Something is wrong with this.  don't use it.
//Invert the TX pair.
//#define INVERTTX

#define BIAS0

#endif

/*
     Modification of "avrcraft" IP Stack.
    Copyright (C) 2014 <>< Charles Lohr
		CRC Code from: http://www.hackersdelight.org/hdcodetxt/crc.c.txt

    Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
