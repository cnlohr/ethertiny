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

#include "sendpack.h"
#include "packetmater.h"

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)

#define OSC20 0xB6
//#define OSCHIGH 0xda  //~25 MHz???
#define OSCHIGH 0xFF  //~30 MHz?


static void setup_clock( void )
{
	/*Examine Page 33*/

	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/


	PLLCSR = _BV(PLLE) | _BV( PCKE );
	PLLCSR |= _BV(LSM);
	OSCCAL = OSC20;

}

//#define SMARTPWR

//Inverting the manchester seems to universally make things worse.
//#define INVERT_MANCHESTER

#ifndef INVERT_MANCHESTER
//Do not split this across byte-addressing boundaries.
char ManchesterTable[16] __attribute__ ((aligned (16))) = {
	0b10101010, 0b01101010, 0b10011010, 0b01011010,
	0b10100110, 0b01100110, 0b10010110, 0b01010110,
	0b10101001, 0b01101001, 0b10011001, 0b01011001,
	0b10100101, 0b01100101, 0b10010101, 0b01010101,
};
#else
//Inverted
char ManchesterTable[16] __attribute__ ((aligned (16))) = {
	0b01010101, 0b10010101, 0b01100101, 0b10100101,
	0b01011001, 0b10011001, 0b01101001, 0b10101001,
	0b01010110, 0b10010110, 0b01100110, 0b10100110,
	0b01011010, 0b10011010, 0b01101010, 0b10101010,
};
#endif
/*
//Debug
char ManchesterTableDEBUG[16] __attribute__ ((aligned (16))) = {
	0b00000000, 0b11000000, 0b00110000, 0b11110000,
	0b00001100, 0b11001100, 0b00111100, 0b11111100,
	0b00000011, 0b11000011, 0b00110011, 0b11110011,
	0b00001111, 0b11001111, 0b00111111, 0b11111111,
};
*/
unsigned char sendbuffer[370];

void SendTestASM( const unsigned char * c, uint8_t len );
int MaybeHaveDataASM( unsigned char * c, uint8_t lenX2 ); //returns the number of pairs.

/*
void MaybeHaveDataTest( unsigned char * c, uint8_t lenX2 ) //LenX2 = 128->256, i.e. put in half the size.
{
	while( lenX2-- )
	{
		*(c++) = USIBR;
		*(c++) = USIBR;
	}
}*/

void waitforpacket( unsigned char * buffer, uint16_t len, int16_t ltime )
{
		OSCCAL = OSCHIGH;

	//Make sure we're not walking in on something.
	while( ltime-- > 0 )
	{
		if( USIBR == 0x00 ) break;
		if( USIBR == 0xFF ) break;
		NOOP;
	}


	while( ltime-- > 0 )
	{
		if( USIBR && (USIBR != 0xFF ) )
		{
			int r = MaybeHaveDataASM( buffer, len );
			ltime-=(len-r)*4+3; //About how long the function takes to execute.
			break;
		}
//		NOOP;
//		NOOP;
//		NOOP;
	}

	while( ltime-- > 0 )
	{
		NOOP;
		NOOP;
		NOOP;
		NOOP;
		NOOP;
	}

	OSCCAL = OSC20;
}


int main( )
{
	int i;
	cli();

	setup_clock();

	DDRB = _BV(1);

	//1st let's see how fast we can clock the pin.


	TCCR0A = _BV(WGM01);
	TCCR0B = _BV(CS00);
	OCR0A = 0;

	USICR = _BV(USIWM0) | _BV(USICS0) | _BV(USITC);

	for( i = 0; i < PacketABytes; i++ )
	{
		sendbuffer[i] = pgm_read_word( &PacketA[i] );
	}

	i = 0;

	int frame = 0;

	struct EthernetPacket * sbe = (struct EthernetPacket*)sendbuffer;

//	PORTB |= _BV(0);  //Enable pullups.
	PORTB &= ~_BV(0); 
	DDRB &= ~_BV(0);
	PORTB &= ~_BV(1);
	USICR &= ~_BV(USIWM0);  //Disable USICR

	while(1)
	{
	//	SendTestASM( sendbuffer, PacketABytes/4 + 3 ); //MUST BE DIVISIBLE BY 2 # of bytes.
	//	continue;

#define LIMITSIZE  sizeof( sendbuffer )/2-50
//#define LIMITSIZE 10

		waitforpacket(sbe->payload, LIMITSIZE, 20000); //wait for 2048 cycles (30MHz/8 = 3.75MHz / 30000 = 8ms)
//		_delay_ms(8);
#ifdef SMARTPWR
		DDRB |= _BV(1);
#endif
		PORTB|=_BV(1);
		NOOP;
		PORTB &=~_BV(1);
#ifdef SMARTPWR
		DDRB &= ~_BV(1);
#endif
		waitforpacket(sbe->payload, LIMITSIZE, 20000); //wait for 2048  (30MHz/8 = 3.75MHz / 30000 = 8ms)
// 		_delay_ms(8);

		i++;

		if( i == 20 )
		{
#ifdef SMARTPWR
			DDRB |= _BV(1);
#endif

			//UDP Data starts at byte #50
			struct EthernetPacket * sbe = (struct EthernetPacket*)sendbuffer;
//			sbe->payload[0] = 0xBB;
//			sbe->payload[1] = frame++;
			sbe->addyfrom = 0x450a000a;
			int rr = Ethernetize( sendbuffer, PacketABytes, 300);

			SendTestASM( sendbuffer, rr/4 + 3 ); //MUST BE DIVISIBLE BY 2 # of bytes.
#ifdef SMARTPWR
			DDRB &= ~_BV(1);
#endif
			i = 0;
		}
	}



	return 0;
} 
