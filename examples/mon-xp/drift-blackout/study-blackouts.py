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
import numpy as np

if len(sys.argv) != 2:
    print("usage: %s PACKET.STATUS" % (sys.argv[0],))
    sys.exit(1)

# Type to specify wether the packet has been lost or received by the destination.
class PacketStatus(object):
    pass
class PacketReceived(PacketStatus):
    pass
class PacketLost(PacketStatus):
    pass

class Packet(object):
    def __init__(self, line):
        line = line.split(' ')

        if line[0] == "RECEIVED":
            self.status = PacketReceived
        elif line[0] == "LOST":
            self.status = PacketLost
        else:
            print("error: invalid packet status '%s'" % (line[0],))
            sys.exit(1)
        self.time   = int(line[1]) # Simulation time for sent packet that was received/lost (in microsec).
        self.seqno  = line[2]      # Sequence number of the packet (warning: seqno can be duplicated over the entire trace).

    def delta(self, packet):
        return packet.time - self.time

class Blackout(object):
    def __init__(self, initial_packet):
        self.packets = [initial_packet]
        self.deltas  = []
        self.single  = True

    def len(self):
        """
        Number of packets in this blackout.
        Use self.single to know if there is a single packet
        in this blackout.
        """
        return len(self.packets)

    def delta_avg(self):
        """ Average distance between packets. """
        if not self.single:
            return np.average(self.deltas)
        else:
            return 0

    def delta_std(self):
        """ Standard deviation of the distance between packets. """
        if not self.single:
            return np.std(self.deltas)
        else:
            return 0

    def delta2last(self, pkt):
        """ Distance to last packet of the blackout. """
        return self.packets[-1].delta(pkt)

    def median_packet(self):
        """
        Return the median packet.
        That is the packet in the middle of the blackout.
        """
        return self.packets[ len(self.packets) // 2 ]

    def center_time(self):
        """
        Return the middle time of the blackout.
        """
        return (self.packets[0].time + self.packets[-1].time) / 2.

    def duration(self):
        """
        Return the duration of the blackout (in µs).
        """
        return self.packets[-1].time - self.packets[0].time

    def median_distance(self, blackout):
        """
        Distance from this blackout center to another.
        Use the median packet as the center.
        """
        return self.median_packet().delta(blackout.median_packet())

    def time_distance(self, blackout):
        """
        Distance from this blackout center to another.
        Use the middle time of the blackout as center.
        """
        return blackout.center_time() - self.center_time()

    def add(self, pkt):
        """
        Add a packet to the blackout.
        All internal values are updated.
        """
        self.single = False
        delta = self.delta2last(pkt)

        self.packets.append(pkt)
        self.deltas.append(delta)

    def close(self):
        """ Declare that the blackout has ended. """
        # Not used for now.
        pass

    def __str__(self):
        pkt_avg  = float(self.delta_avg()) / 1e6
        pkt_std  = float(self.delta_std()) / 1e6
        median   = self.median_packet()
        center   = self.center_time()
        duration = float(self.duration()) / 1e6

        return "len=%d ; duration=%.3f s ; center=%.1f µs ; pkt_delta = (%.3f +/- %.3f) s" % (self.len(), duration, center, pkt_avg, pkt_std)

    def plot_str(self):
        # LEN DURATION(us) CENTER-TIME(us) MEDIAN-TIME(us) AVG-PACKETS-DELTA(us) STD-PACKETS-DELTA(us)
        return "%d %d %f %d %f %f" % (self.len(), self.duration(), self.center_time(), self.median_packet().time, self.delta_avg(), std.delta_std())

# Load all packets into memory.
packets = []
with open(sys.argv[1]) as f:
    for line in f:
        line = line.strip()
        if len(line) == 0 or line[0] == '#':
            continue
        packets.append(Packet(line))


"""
We can have multiple blackouts classifiers.
It depends on what you count as a blackout.
In the previous iteration of this scripts
that used only lost packets, we had one that
classified blackouts on the average distance
and another on the max distance between the
packets.
"""

def seq_classify_blackouts(packets):
    """
    Blackouts are seen as a series of consecutive
    lost packets. Nothing more, nothing less.
    """

    blackouts = []
    current   = None
    for packet in packets:
        if packet.status == PacketLost:
            if current == None:
                current = Blackout(packet)
            else:
                current.add(packet)
        elif current:
            current.close()
            blackouts.append(current)
            current = None
    if current:
        current.close()
        blackouts.append(current)

    return blackouts

def lenient_classify_blackouts(packets, max_blackout_receive = 2):
    """
    Blackouts are seen as a series of lost packets with at most
    max_blackout_receive packets between each packet.

    More received packets ends the blackout.
    """

    blackouts = []
    current   = None
    recv_left = -1

    for packet in packets:
        if packet.status == PacketLost:
            if current == None:
                current = Blackout(packet)
            else:
                current.add(packet)
            recv_left = max_blackout_receive
        else:
            if recv_left > 0:
                recv_left -= 1
            elif current:
                current.close()
                blackouts.append(current)
                current = None
    if current:
        current.close()
        blackouts.append(current)

    return blackouts


# Classify and filter blackouts if necessary.
blackouts = lenient_classify_blackouts(packets)
blackouts = filter(lambda b : not b.single, blackouts)

# Show off everything
last = None
median_distances = []
time_distances   = []
durations        = []
for b in blackouts:
    if last:
        median_dist = float(last.median_distance(b)) / 1e6
        time_dist   = float(last.time_distance(b)) / 1e6
        duration    = float(b.duration()) / 1e6

        # The commented 'if' there filter the outliers
        # caused by the 125ms CCAs cycle drift.
        #if median_dist > 50:
        median_distances.append(median_dist)
        #if time_dist > 50:
        time_distances.append(time_dist)
        durations.append(duration)

        print("%s ; median_dist=%.3f s ; time_dist=%.3f s" % (str(b), median_dist, time_dist))
    else:
        print(b)
    last = b

def print_avgstd(msg, unit, xs):
    print("%s: (%.3f +/- %.3f) %s" % (msg, np.average(xs), np.std(xs), unit))

print_avgstd("Median distance between blackouts", 's', median_distances)
print_avgstd("Time distance between blackouts  ", 's', time_distances)
print_avgstd("Blackouts duration               ", 's', durations)
