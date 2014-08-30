//Packet mater.  A super tiny file that helps with checksums and ethernet CRCs.
//CRC Was taken from linked page.

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
#include "packetmater.h"

//Fix all checksums for UDP packets and add etherlink CRC.
uint16_t Ethernetize( unsigned char * packet, int plen, int udplenoverride )
{
	plen -= 12;
	packet += 8; //User must provide preamble.

	if( (udplenoverride+8) > plen-34 )
		plen = (udplenoverride+8) + 34;

	//Need to buffer to 4-byte boundaries.
	plen = ((plen - 1)&0xfffc) + 4;

	if( packet[12] == 0x08 && packet[13] == 0x00 )
	{
		uint16_t fullpacklen = plen-14;
		packet[16] = fullpacklen>>8;
		packet[17] = fullpacklen&0xff;
		packet[24] = 0;
		packet[25] = 0;
		uint16_t csumr;
//		fprintf( stderr, "Header checksum: 0x%04x\n", csumr = csrange(14,20) );
		csumr = internet_checksum( &packet[14], 20 );

		packet[24] = csumr>>8;
		packet[25] = csumr&0xff;

		if( packet[23] == 0x11  )
		{
			uint16_t udppacklen = udplenoverride + 8;//fullpacklen-20;
			packet[32+6] = udppacklen>>8;
			packet[32+7] = udppacklen&0xff;

			uint16_t pseudo = 0x11 + 0x8 + udppacklen-8; //UDP number + size + length (of packet)
			packet[40+0] = pseudo>>8;
			packet[40+1] = pseudo&0xff;

			csumr = internet_checksum( &packet[26], udppacklen+8 );
			if( csumr == 0 ) csumr = 0xffff;
			packet[40+0] = csumr>>8;
			packet[40+1] = csumr&0xff;
		}
	}


	uint32_t crc = crc32b( 0, packet, plen );

	packet[plen+0] = crc & 0xff;
	packet[plen+1] = (crc>>8) & 0xff;
	packet[plen+2] = (crc>>16) & 0xff;
	packet[plen+3] = (crc>>24) & 0xff;

	return plen;
}


//From: http://www.hackersdelight.org/hdcodetxt/crc.c.txt
uint32_t crc32b(uint32_t crc, unsigned char *message, int len) {
   int i, j;
   uint32_t mask;
	uint8_t byte;

   i = 0;
//   crc = 0xFFFFFFFF;
	crc = ~crc;
   while (i < len) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

uint16_t internet_checksum( const unsigned char * start, uint16_t len )
{
	uint16_t i;
	const uint16_t * wptr = (uint16_t*) start;
	uint32_t csum = 0;
	for (i=1;i<len;i+=2)
	{
		csum = csum + (uint32_t)(*(wptr++));	
	}
	if( len & 1 )  //See if there's an odd number of bytes?
	{
		uint8_t * tt = (uint8_t*)wptr;
		csum += *tt;
	}
	while (csum>>16)
		csum = (csum & 0xFFFF)+(csum >> 16);
	csum = (csum>>8) | ((csum&0xff)<<8);
	return ~csum;
}



