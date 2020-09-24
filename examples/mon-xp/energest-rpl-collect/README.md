Description
===========

Use a RPL IPv6 collect protocol and measure the OFF delay on a relay node.

        _ 3
S -- 2 /
       \_ 4

The delay is measured on node 2. The Sink node has its RDC disabled.
The sender nodes send there sensors values randomly from 0 to 10 seconds.

The duration of the experiment is ~650 s (but 600 for energest).

Results
=======

!! One message per ~10 second instead of
   one message per seconde for broadcast energest !!

Energest value for
SEED=1337
TIME=650s (energest on 600s)

FIXME: why TX != 0 here ?

      (mW)           (mW)
      without ISR    with ISR
CPU   0.467          0.466
LPM   0.165          0.165
IRQ   0.129          0.130
TX    0.170          0.170
RX    0.789          0.745
UART  0              0
total
