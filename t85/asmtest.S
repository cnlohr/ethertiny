// Copyright 2012 Charles Lohr under the MIT/x11, newBSD, LGPL or GPL licenses.  You choose.

//XXX TODO In SPIW, should we be setting ETSI low on start in r18??

#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0

#include <avr/io.h>

.global SendTick
SendTick:
	ori r18, 0b00010101
	ori r19, 0b00001101
	ori r20, 0b00000101
	out PORTB, r19
	out PORTB, r19
	out PORTB, r18
	out PORTB, r18
	out PORTB, r20
	ret

