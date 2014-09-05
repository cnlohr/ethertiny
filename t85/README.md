This program just bit bangs data out a differential pair.
Whatever you put as your packet, it will parrot out at 
10 Mbit/sec.

The idea is you modify testpack.txt to be the following from:

:Function Name

[Ethernet bytes in hexadecmil, two letters each]

 ; Anything after a [space] and a semicolon is a comment.


Then, it will create "sendpack.S" with all of the assembly
to go run everything all at once.  In order to send a packet,
simply call the function to send a packet!


If you need to use the other ports for I/O, you'll need
to modify the code to set up the right bits on port B.

Right now, port PB3,PB4 are the TX+/TX- pins.
A switch can be connected between PB0 and PB1
and another switch may be connected btween PB2 and PB1.


