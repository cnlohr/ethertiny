WARNING

This is a really dangerous project.  Use at your own risk.

By directly ethernet without magnetics to an AVR, you may
cause serious damage, fire, or anything else crazy if there
is a DC offset.  It does not follow 802.3 standards for
10Base-T Ethernet


It has two projects in it:
 t85: uses differential signaling with fixed packets, similar to another
	person's project, igorplug, found here:
	http://www.cesko.host.sk/IgorPlugUSB/IgorPlug-USB%20%28AVR%29_eng.htm

 t85spi: uses the USI, and can send/receive arbitrary packets.

Schematics are in the 

See the youtube video for more info:
T85 Video:    http://youtu.be/mwcvElQS-hM
T85spi Video: [will post]

Paper on T85spi is found in the paper/ folder.





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
