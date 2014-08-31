#ifndef _ETH_CONFIG_H
#define _ETH_CONFIG_H

//Terrifying though it may be... This is how it's all configured.

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
//I.E. a buffer size of 410 will support roughly 94 bytes on-wire.

#define ETBUFFERSIZE 410
#define MAX_FRAMELEN (ETBUFFERSIZE-30)
#define TX_SCRATCHES 1
#define RX_BUFFER_START 0
#define RX_BUFFER_END   MAX_FRAMELEN


//OSC20 will depend on the specific chip.  Needs to be AS CLOSE TO 20 MHZ AS POSSIBLE!
//TUNE THIS!
#define OSC20 0xb5

//OSCHi needs to be as fast as the chip goes, but less than ~35 MHz. IT MUST EXCEED 30 MHZ!
#define OSCHIGH 0xFF  //~30 MHz?

#define INCLUDE_UDP



#endif

