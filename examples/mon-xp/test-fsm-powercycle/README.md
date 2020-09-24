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

The goal of this experiment is to test that the FSM powercycle is compatible with
the vanilla ContikiMAC powercycle. Results are discarded.

Results
=======
