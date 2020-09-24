Description
===========

Similar to the param-process-lock-poll experiment.
In this case the balanced process is parametrized and the experiment started numerous
so that we can correlate the process working time with the average radio off delay.

Also we measure the energy consumption in the balanced process after all packets have been received.

Note it it reads ISR2POLL sometimes but it is truly ISR2OFF that is measured.

All experiments are done with 100 probes with a duration of 120seconds.

NOTE!!!! There is an error in the legend of the primary results.
Read xp.sh to be sure.

We also display statistics for the ISR. By this we mean the time passed (in number of CPU cycles)
in the ISR of the CC2420 radio.

Results
=======

Do gnuplot mW-<x>.gnuplot for results.


1) You can see on mW-listen that the optimization effectively reduce the power consumption of the node.
from 0.077 mW @~0 cyc. to 5.72 mW @~650000 cyc.

Note the drop at ~650000 cyc. is due to a overflow error in the experiment code (in balanced node firmware implementation).

2) In mW-IRQ we see that the IRQ takes a bit more time with the optimization. Not much however, only 0.5µW.
I cannot explain why we spend more time in the IRQ with lower cycles. Is it the clock?

3) Intriguing in mW-CPU, we see that we have lower performance with the optimization enabled.
However this does not take into account the radio power. So this is due to the IRQ and the CtkMAC modifications.
The difference is only of ~2-3nW. Note that if this take into account the IRQ time as we have seen before.
This means that the time we spend in IRQ is largely compensated by the optimization we have done by factoring CtkMAC.

However this could only be attributed to compiler pecularities.

FIXME: sure that Energest CPU time measures also IRQ. If not then this increased time is from the factorisation
       of the isr_off() code in CtkMAC.

We can see that better on the mW-combined figure where we see clearlly that radio dominates power consumption.

4) Finally mW-improve show the relative improvement in total power (%) that is:

100 * (Tdisab. - Tenab.) / Tdisab.

We see that we have 1.14% improvement in the worst case (no external cycles).
We have 2% improvement on 6k cycles which is not really convincing.
On 70k cycle we start to see reasonable power improvement with 10%.

5) mW-improve_listen we see the ratio of time for the listen time (RADIO TIME) with/without the optimization.
We see a 17.5% improvement below 5k cyc.

===

CCl. effectively we see an optimization but it is only consequent for very large working cycles.
We have improvement if we look at the ratio of listen time.

Results IRQ var.
================

The average minimum and maximum number of cycles per RADIO ISR is constant for enabled/disabled optimization.
A bit larger for the optmized version.

1) Look at mW-IRQ-CC240-TIME-variance.

We see that the sum IRQ time is constant for the optimized version, but there is variance for the non optimized version.
The radio effectively receives more messages in the non optimized version. You can see this by looking at the
number of ISR/POLL field (field #1 / #2). We also see that this effect is slightly larger for lower cycles.
With a maximum at around 11k working cyc.


1a) PCAP

To investigate this we look at the PCAP. We temporarily enabled the PCAP logger plugin to look at the trace for
once simulation instance. Note that the PCAP logger is not enabled in our instance of Cooja (the code is not there,
and we cannot import it easily) so we use the Radio logger 6LoWPAN PCAP analyzer.

What we see is that both with optimization enabled/disabled we see exactly the same number of messages. That is exactly
the same messages on the air. Message are repeated during their full wake-up period.

1b) Up to the MAC

We receive packets more packets than we should. Do we receive them up through the CtkMAC layer?
So I instrumentalized the input() code of CtkMAC to see where each packet was going.

One XP @10k cyc.
With ISR disabled we go 134 in the input_send()
With ISR enabled  we go 100 in the input_send()

With optimization disabled => 13 are duplicated packets.

Two explanations:

 1) we see more messages because by messages we count the "POLL" events
    that still happen in the ISR. Since the RADIO is not shutted OFF 
    without the optimization, we still receive duplicated broadcast messages.
    which are counted as newly received messages.

 2) since the radio is not shut off, if we receive a message between the moment
    the polled netstack process has been started, and the moment this process
    effectively put the radio back to sleep, the process is rescheduled and
    we see a duplicate.

    (acount for only 13 out of 34 messages)
