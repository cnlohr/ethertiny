#include "hlprocess.h"

uint16_t lowct;
uint16_t hict;

//Note: -1 indicates unmatched short pulse.  I.e. quick transitions must pair up.
int16_t Demanchestrate( unsigned char * raw_data_packet, uint16_t max_packet_data_len )
{
	uint8_t in_preamble = 4;
	uint8_t t;
	uint8_t mask;

	uint8_t bit;
	uint8_t lastbit = 0;
	int8_t count = 0;
	uint16_t plen = 0;
	uint8_t shortflag = 0;

	uint8_t cemitbit = 1; //current emitting bit.
	uint8_t cemitbyte = 0;
	uint8_t cemitmask = 1;

	uint8_t * output = raw_data_packet;

	lowct = 0;
	hict = 0;

	while( 1 )
	{
		t = *(raw_data_packet++);
		for( mask = 0x80; mask; mask>>=1 )
		{
			bit = (t&mask)?1:0;
			if( bit )
			{
				hict++;
			}
			else
			{
				lowct++;
			}
			if( bit != lastbit )
			{
				//Ignore first two changes.
				if( in_preamble > 2 && count > 1 )
				{
					in_preamble--;
				}
				else if( in_preamble <= 2 && count < 2 && in_preamble )
				{
					in_preamble--;
				}
				else if( !in_preamble )
				{
					uint8_t mark_emit = 0;

					//Something happened.
					//If it's immediate, i.e. count = 0, count = 1 -> short (mark for ticking)
					//If it's later, i.e. count = 2, count = 3 -> long (invert)
					//We cannot get >3 as that's handled when we increase count.
					if( count > 1 )
					{
						if( shortflag )
						{
							return -1;
						}
						//invert bit.
						cemitbit = !cemitbit;

						mark_emit = 1;
					}
					else
					{
						if( shortflag )
						{
							mark_emit = 1;
							shortflag = 0;
						}
						else
						{
							shortflag = 1;
						}
					}
					if( mark_emit )
					{
						//Emit cemitbit. 
						cemitbyte |= cemitbit?cemitmask:0;
						cemitmask <<= 1;

						if( (cemitmask&0xff) == 0 )
						{
							output[plen++] = cemitbyte;
							cemitbyte = 0;
							cemitmask = 1;
						}
						mark_emit = 0;
					}
				}

				lastbit = bit;
				count = 0;
			}
			else
			{
				count++;
				if( count > 3 )
				{
					return plen;
				}
			}
		}
		//Next byte.
	}
}

