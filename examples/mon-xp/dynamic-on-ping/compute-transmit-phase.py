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

# monitor_record() offset in microseconds.
# Warning! May change from one config to another.
# In particular with different CPU speed and drift enabled.
MONITOR_OFFSET_US = 6.917

if len(sys.argv) != 2:
    print("usage: %s TRACE-FILE(0xbbaa)" % (sys.argv[0],))
    print("Expect 0xbbaa at the origin of the transmit.")
    sys.exit(1)

class ParseLine(object):
    def __init__(self, s):
        s = s.split(' ')

        # Parse sim time
        sim_time = s[2]
        sim_time = sim_time.split('=')
        assert sim_time[0] == "SIM_TIME_US"
        self.sim_time = int(sim_time[1])

        # Parse state
        state = s[-1]
        state = state.split('=')
        assert state[0] == "STATE"
        self.state = state[1].strip('\n')

    def __str__(self):
        return "%d %s" % (self.sim_time, self.state)

# Interval from one transmit to another
intervals = []
txend = None
with open(sys.argv[1]) as f:
    for line in f:
        event = ParseLine(line)

        if event.state == "bbaa":
            if txend:
                print("%d" % (event.sim_time - txend.sim_time))
                intervals.append(event.sim_time - txend.sim_time)
            txend = event

#map(lambda t : t - MONITOR_OFFSET_US, intervals)

# Filter times > 10ms.
#intervals = list(filter(lambda t : t < 10e3, intervals))

print("Num transmits: %d" % (len(intervals),))
print("Average transmit duration: (%f +/- %f) µs" % (np.average(intervals), np.std(intervals)))
print("Min transmit duration: %f µs" % (min(intervals)))
print("Max transmit duration: %f µs" % (max(intervals)))
print("Percentiles:")
for p in (5, 25, 50, 75, 95):
    print(" %dth: %f" % (p, np.percentile(intervals, p)))
