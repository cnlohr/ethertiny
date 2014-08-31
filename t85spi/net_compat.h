#ifndef _ENC424JCOMPAT_H
#define _ENC424JCOMPAT_H

//enc424j600 compat lib.
//We're using the IP library from AVRCraft... but we're using our own PHY+MAC

//TODO: Should probably use Timer1 (since we can't use Timer0)
//to make sure we send media detect pulses at the right points.
//Also, as a user, you cannot use the USI or Timer0

#include <stdint.h>
#include "eth_config.h"

//MyMAC is included in this driver.
extern unsigned char MyMAC[6];
extern unsigned char ETbuffer[ETBUFFERSIZE];
extern unsigned short ETsendplace;
//For telling where the current transaction started.
extern uint16_t sendbaseaddress;
extern unsigned short ETchecksum;

//Do not split this across byte-addressing boundaries.
//We do some fancy stuff when we send the manchester out.
extern char ManchesterTable[16];

//return 0 if OK, otherwise nonzero.
int8_t et_init( const unsigned char * macaddy );

int8_t et_xmitpacket( uint16_t start, uint16_t len );

//This waits for 8ms, sends an autoneg notice, then waits for 8 more ms.
//Ordinarily this would if packets were processed and still ned processing, but
//that doesn't make sense for this driver.  Do not put this in a loop unto itself.
unsigned short et_recvpack();

//You have to write this! (Or the underlying IP core must)
void et_receivecallback( uint16_t packetlen );

//Finish up any reading. 							//CLOSURE
static inline void et_finish_callback_now() { }

//Raw, on-wire pops. (assuming already in read)
void et_popblob( uint8_t * data, uint8_t len );
static inline void et_dumpbytes( uint8_t len ) { ETsendplace += len; }
static inline uint16_t et_pop16() { return ETbuffer[ETsendplace++]; }
static inline uint8_t et_pop8() { return ETbuffer[ETsendplace++]; }

//Raw, on-wire push. (assuming already in write)
void et_pushpgmstr( const char * msg );
void et_pushstr( const char * msg );
void et_pushblob( const uint8_t * data, uint8_t len );
void et_pushpgmblob( const uint8_t * data, uint8_t len );
static inline void et_push8( uint8_t d ) { ETbuffer[ETsendplace++] = d; }
static inline void et_pushzeroes( uint8_t nrzeroes ) { while( nrzeroes-- ) et_push8(0); }
void et_push16( uint16_t v );

//Start a new send.									//OPENING
static inline void et_startsend( uint16_t start ) { sendbaseaddress = ETsendplace = start; }

static inline uint16_t et_get_write_length() { return ETsendplace - sendbaseaddress; }

//End sending (calls xmitpacket with correct flags)	//CLOSURE
static inline void et_endsend() { et_xmitpacket( sendbaseaddress, ETsendplace - sendbaseaddress ); }

//Deselects the chip, can halt operation.
static inline void et_stopop() { }

//Start a checksum
void et_start_checksum( uint16_t start, uint16_t len );

//Get the results of a checksum (little endian)
static inline uint16_t et_get_checksum() { return ETchecksum; }

//Modify a word of memory (little endian)
static inline void et_alter_word( uint16_t address, uint16_t val ) { ETbuffer[address] = val>>8; ETbuffer[address+1] = val & 0xff; }

//Copy from one part of the enc to the other.
//Warning range_end is INCLUSIVE! You will likely want to subtract one.
void et_copy_memory( uint16_t to, uint16_t from, uint16_t length, uint16_t range_start, uint16_t range_end );

//Low-level access (not available)
static inline void et_write_ctrl_reg16( uint8_t addy, uint16_t value ) { }
static inline uint16_t et_read_ctrl_reg16( uint8_t addy ) { return 0; }




#endif

