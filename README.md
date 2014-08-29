WARNING

This is a really dangerous project.  Use at your own risk.

By directly ethernet without magnetics to an AVR, you may
cause serious damage, fire, or anything else crazy if there
is a DC offset

See the youtube video for how to hook it up.
     http://youtu.be/mwcvElQS-hM


You are still responsible for sending the NLP pulses every
16 +/- 8ms.


---------------For the Tiny85 version----------------

The idea is you modify testpack.txt to be the following from:

:Function Name

[Ethernet bytes in hexadecmil, two letters each]

 ; Anything after a [space] and a semicolon is a comment.


Then, it will create "sendpack.S" with all of the assembly
to go run everything all at once.  In order to send a packet,
simply call the function to send a packet!


If you need to use the other ports for I/O, you'll need
to modify the code to set up the right bits on port B.

Right now, port PB3,PB4 are the TX+/TX- pins







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
