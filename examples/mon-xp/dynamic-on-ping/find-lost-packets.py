#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#  Copyright (c) 2019, David Hauweele <david@hauweele.net>
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this
#      list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys

# Two messages with the same seqno must be within this value to be considered
# the same for sender and receiver. This value is given in microsecs.
# Thus 10seconds implies that packets needs to be sent faster than 1 pkt per 0.2ms
# for missing a lost packets, and also that packets have 10 seconds to transit from
# sender to destination (remember that this XP is direct unicast, without any forwarding).
LIMIT_SEQNO_TIME = 10e6

class ParseLine(object):
    def __init__(self, s):
        s = s.split(' ')

        # Parse sim time
        sim_time = s[2]
        sim_time = sim_time.split('=')
        assert sim_time[0] == "SIM_TIME_US"
        self.sim_time = int(sim_time[1])

        # Parse state
        seqno = s[-1]
        seqno = seqno.split('=')
        assert seqno[0] == "STATE"
        self.seqno = seqno[1].strip('\n')

    def __str__(self):
        return "%d %s" % (self.sim_time, self.seqno)

# Type to specify wether the packet has been lost or received by the destination.
class PacketStatus(object):
    pass
class PacketReceived(PacketStatus):
    pass
class PacketLost(PacketStatus):
    pass

if len(sys.argv) != 3:
    print("usage: %s SRC-NODE-TRACE_ENT=TEST.TXT DST-NODE-TRACE_ENT=TEST.TXT" % (sys.argv[0],))
    sys.exit(1)

recv_packets = {}
packets = [] # contains tuples (PacketStatus, event), event is a ParseLine object.

# dst_event
with open(sys.argv[2]) as f:
    for line in f:
        event = ParseLine(line)

        if event.seqno not in recv_packets:
            recv_packets[event.seqno] = [event]
        else:
            recv_packets[event.seqno].append(event)


# src_event
with open(sys.argv[1]) as f:
    for line in f:
        event = ParseLine(line)

        if event.seqno not in recv_packets:
            # Sent packet seqno not in received packets ?
            # Then its definitely lost.
            packets.append((PacketLost, event))
        else:
            # If the source sent packet seqno is in received packets,
            # it may be because seqno are repeating themselves.
            # So we check if there is a received packet with same seqno
            # that is within a 10s limit.
            # Since seqno increase by one on each packet and we send
            # one packet per second, this 10s limit is well below the
            # possible range of value for the seqno (in other words,
            # there is no false positive received packet).
            found = None
            for dst_event in recv_packets[event.seqno]:
                # Message with same seqno should be within one second
                # from this one. Otherwise it is not considered to be
                # the same packet.
                delta = dst_event.sim_time - event.sim_time
                if delta >= 0 and delta < LIMIT_SEQNO_TIME:
                    found = dst_event
            if found:
                # Src and recv packets were found with same seqno
                # and within 10s of each other.
                # That's a received packet.
                packets.append((PacketReceived, event, found))
            else:
                # No packet was found near this one.
                # This packet is lost.
                packets.append((PacketLost, event))

# sort lost packets by simulation time
packets.sort(key = lambda t : t[1].sim_time)

print("# LOST     src_time(us) src_seqno")
print("# RECEIVED src_time(us) src_seqno dst_time(us) dst_seqno time_to_reception(us)")
print("# time_to_reception is from just before:")
print("#   uip_udp_packet_send() in sender.")
print("# and just after receiving data in the application layer for the receiver.")
for t in packets:
    if t[0] == PacketLost:
        print("LOST %s" % (str(t[1]),))
    elif t[0] == PacketReceived:
        delta = t[2].sim_time - t[1].sim_time
        print("RECEIVED %s %s %d" % (str(t[1]), str(t[2]), delta))
    else:
        # Unknown PacketStatus.
        # Should never land here.
        assert 0

