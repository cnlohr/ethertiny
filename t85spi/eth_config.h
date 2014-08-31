#ifndef _ETH_CONFIG_H
#define _ETH_CONFIG_H

//Terrifying though it may be... This is how it's all configured.

//Warning: YOU Must use with an ATTinyX5 series chip or another chip
//that allows you to tune OSCCAL to a high enough frequency.

//On the ATTiny85, you have 512bytes of RAM.  
// The stack uses ETBUFFERSIZE + 16 (Manchester table) + 6 (internal states) + 6(MAC, but not in core)
//(We need to keep the manchester in RAM)

//Warning: When receiving packets, you will only be able to receive packets
// less than 1/3 the size of preamble + etbuffersize.

#define ETBUFFERSIZE 320


//OSC20 will depend on the specific chip.  Needs to be AS CLOSE TO 20 MHZ AS POSSIBLE!
//TUNE THIS!
#define OSC20 0xBb

//OSCHi needs to be as fast as the chip goes, but less than ~35 MHz. IT MUST EXCEED 30 MHZ!
#define OSCHIGH 0xFF  //~30 MHz?




#endif

