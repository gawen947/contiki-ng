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

# Maximum distance between transmit and CCA in microsecs.
MAX_LENGTH        = 10e3

if len(sys.argv) != 3:
    print("usage: %s (distrib-bin,0=>time-evolution) TRACE-FILE" % (sys.argv[0],))
    print("Expect 0xccaa after TRANSMIT and 0xccab before first CCA")
    sys.exit(1)

try:
    mode = int(sys.argv[1])
except:
    print("error: first argument must be an integer (size of distrib bin or zero for time evolution).")
    sys.exit(1)

class ParseLine(object):
    def __init__(self, s):
        s = s.split(' ')

        # Parse NID
        nid = s[3]
        nid = nid.split('=')
        assert nid[0] == "NID"
        self.nid = int(nid[1])

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

t2c = {}
txend = None
with open(sys.argv[2]) as f:
    for line in f:
        event = ParseLine(line)

        if event.state == "ccaa" and event.nid == 1:
            txend = event
        elif event.state == "ccab" and event.nid == 2:
            try:
                d = event.sim_time - txend.sim_time
                if d < MAX_LENGTH:
                    t2c[txend.sim_time] = d
            except:
                continue
            txend = None


#map(lambda t : t - MONITOR_OFFSET_US, t2c)

if mode == 0:
    # FIXME: are the dictionary keys sorted or given in the order in which they were inserted ?
    # When parsing the trace, they should be loaded already sorted.
    print("# TRANSMIT-TIME(us) TRANSMIT-2-CCA-DURATION(us)")
    for t,d in t2c.items():
        print("%d %d" % (t, d))
else:
    print("# TRANSMIT-2-CCA-DURATION(us) OCCURENCES  (bin=%d)" % (mode,))
    bins = {}

    for t,d in t2c.items():
        d = int(float(d) / mode) * mode

        if d not in bins:
            bins[d]  = 1
        else:
            bins[d] += 1

    for d,occ in bins.items():
        print("%d %d" % (d, occ))

durations = list(t2c.values())
print("# Num t2cs: %d" % (len(durations),))
print("# Average t2c duration: (%f +/- %f) µs" % (np.average(durations), np.std(durations)))
print("# Min t2c duration: %f µs" % (min(durations)))
print("# Max t2c duration: %f µs" % (max(durations)))
print("# Percentiles:")
for p in (5, 25, 50, 75, 95):
    print("#  %dth: %f" % (p, np.percentile(durations, p)))
