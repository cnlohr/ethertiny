#ifndef _HLPROCESS_H
#define _HLPROCESS_H

#include <stdint.h>

//Tool for traking a high speed serial stream from the USI and converting it
//into properly organized packets.

//WARNING: You _MUST_ sample at MORE THAN 30MHz.  Aliasing will happen below
//that frequency.  You ALSO must bias your signal so that you have roughly as
//many 1's as 0's.

//This particular parser is hard coded to work with between ~31 and ~38
//MHz.  This is done because quick bit changes in the ethernet signal show up
//as 1 or 2 of the same bit type, and longer changes show up as 3 or 4 of the
//same symbol.  More than 4 indicate a packet termiantion.

//Please leave some of the preamble in tact.  It requires a few bytes of
//preamble to properly sync.  You will also need a trailer of at least 5 of
//the same symbol, repeating.

//Steps involved:
// Process the manchester into bit counts.
// Look for the beginning-of-packet flag.
// Watch for changes, and push the bits.
// Depacketize into an output raw packet.

//You pass in the raw data packet, along with the maximum output length
//of the data.  This function will handle automatic termination internally.


//The bitstream must have been read in in MSB first, though Ethernet is LSB
//first.  This is because the USI on the AVR is MSB first.

//Returns # of bytes read.
//If negative, then there was a fault.  Such as an unmatched quick transition
//which is not allowed with manchester encoding.
int16_t Demanchestrate( unsigned char * raw_data_packet, uint16_t max_packet_data_len );

//How many high's & lows to help with biasing.
extern uint16_t lowct;
extern uint16_t hict;


#endif


