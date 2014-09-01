/*

	WARNING: Connecting an AVR to ethernet without magnetics is VERY DANGEROUS.
	It may cause high voltages to go through the AVR and destroy it in DANGEROUS
	that may harm personal safety.  Do only at your own risk.

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "net_compat.h"
//#include "sendpack.h"
#include "packetmater.h"
#include "hlprocess.h"
#include "iparpetc.h"

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


void SendTestASM( const unsigned char * c, uint8_t len );
int MaybeHaveDataASM( unsigned char * c, uint8_t lenX2 ); //returns the number of pairs.

uint8_t MyMAC[6] = { 0x00, 0x55, 0x00, 0x55, 0x00, 0x55 };
uint8_t MyIP[4] = { 192, 168, 0, 153 };
uint8_t MyMask[4] = { 255, 255, 255, 0 };

void HandleUDP( uint16_t len )
{
	//Do nothing (yet)
}


int main( )
{
	int i;
	cli();

	setup_clock();

	//1st let's see how fast we can clock the pin.
	et_init( MyMAC );

	i = 0;

	int frame = 0;


	while(1)
	{
		et_recvpack();

		i++;

		if( i == 20 )
		{
			//How to send a UDP Packet.
			et_stopop();
			et_startsend( 0 );
			memset( macfrom, 0xff, 6 );
			send_etherlink_header( 0x0800 );
			send_ip_header( 0, (unsigned char*)"\xff\xff\xff\xff", 17 ); //UDP Packet to 255.255.255.255
			et_push16( 13312 ); //To port
			et_push16( 1024 ); //from port
			et_push16( 0 ); //length for later
			et_push16( 0 ); //csum for later
			et_pushpgmstr( PSTR( "HELLO!\n" ) ); //csum for later
			util_finish_udp_packet();

			i = 0;
		}
	}



	return 0;
} 
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

