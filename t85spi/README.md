This is a work-in-progress, but I'm trying to see if I can send + receive packets
on an ATTiny.

This one is a little different than the last one.

Hook the RX-, TX- pins to ground.

Hook the TX+ (our TX (out from us)) through a 47nF capacitor to PB1

Hook the RX+ (our RX (in to us)) through a 1nF capacitor to PB0.

TODO: Find a better way to bias PB0. NOTE: PB0 needs to be biased.
