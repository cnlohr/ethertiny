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


