Description
===========

Use unicast with static routing to measure the OFF delay on a relay node.

 (1)    (2)    (3)
 dst -- fwd -- src

The delay is measured on node 2. All nodes have their RDC (ContikiMAC) enabled.
Nodes run ContikiMAC (double checked with UART).
The sender nodes send there sensors values randomly at 10s + [0,5]s

The IP address of each node is aaaa::<node-number>

The duration of the experiment is 1 hour.

With Energest we measure each 10seconds the power usage.

We have two type for node 2. With ISR OFF optimization enabled and disabled.

Results
=======

!! One message per ~10 second instead of
   one message per seconde for broadcast energest !!

Energest value for
SEED=1337
TIME=1200s (energest on 1000s)

FIXME: why TX=0 ?
FIXME: if we do ISR2DEVOFF cycles-without-opt/cycles-with-opt we have ~4.
       but total recept. factor (without/with opt) is ~1.03
       seems OK though, ISR2OFF is not all the time in RX (we also have CCA, ...)

      without ISR    with ISR
CPU   0.03542 mW     0.03506 mW
LPM   0.18 mW        0.18 mW
IRQ   0.1295 mW      0.1298 mW
TX    0              0
RX    0.5602 mW      0.5484 mW
UART  0              0
total 0.90512 mW     0.89326 mW
