/*

	WARNING: Connecting an AVR to ethernet without magnetics is VERY DANGEROUS.
	It may cause high voltages to go through the AVR and destroy it in DANGEROUS
	that may harm personal safety.  Do only at your own risk.

*/

/*
    Copyright (C) 2014 <>< Charles Lohr


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


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "net_compat.h"
#include "sendpack.h"
#include "packetmater.h"
#include "hlprocess.h"

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)

static void setup_clock( void )
{
	/*Examine Page 33*/

	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/


}

//#define SMARTPWR


//unsigned char ETbuffer[390];

void SendTestASM( const unsigned char * c, uint8_t len );
int MaybeHaveDataASM( unsigned char * c, uint8_t lenX2 ); //returns the number of pairs.



int main( )
{
	int i;
	cli();

	setup_clock();

	DDRB = _BV(1);

	//1st let's see how fast we can clock the pin.
	et_init( MyMAC );

	for( i = 0; i < PacketABytes; i++ )
	{
		ETbuffer[i] = pgm_read_word( &PacketA[i] );
	}

	i = 0;

	int frame = 0;

	struct EthernetPacket * sbe = (struct EthernetPacket*)ETbuffer;

	while(1)
	{
	//	SendTestASM( ETbuffer, PacketABytes/4 + 3 ); //MUST BE DIVISIBLE BY 2 # of bytes.
	//	continue;
		et_recvpack();

		i++;

		if( i == 20 )
		{
#ifdef SMARTPWR
			DDRB |= _BV(1);
#endif

			//UDP Data starts at byte #50
			struct EthernetPacket * sbe = (struct EthernetPacket*)ETbuffer;
//			sbe->payload[0] = 0xBB;
//			sbe->payload[1] = frame++;
			sbe->addyfrom = 0x450a000a;
			int rr = Ethernetize( ETbuffer, PacketABytes, 320);

			SendTestASM( ETbuffer + 8, rr/4 + 3 - 2 ); //MUST BE DIVISIBLE BY 2 # of bytes.
#ifdef SMARTPWR
			DDRB &= ~_BV(1);
#endif
			i = 0;
		}
	}



	return 0;
} 
