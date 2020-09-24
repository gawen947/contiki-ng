import sys
import fileinput
import scipy.stats
from collections import deque

BASE_FREQ=3.904173

class Window(object):
    def __init__(self, size):
        """
        Size is the interval of time on which the linear regression is performed.
        It must be given in microseconds.
        """

        self.size  = size
        self.q     = deque()
        self.drift = {} # list of drift values, time in microseconds

    def insert(self, simtime, cputime):
        """ Take simetime and CPU time (given in microseconds both). """

        self.q.append((simtime, cputime))

        if simtime - self.q[0][0] == 0:
            return

        while simtime - self.q[0][0] > self.size:
            self.q.popleft()

        sample_time = self.q[0][0] + (simtime - self.q[0][0]) / 2.0
        simtimes_x, cputimes_y = list(zip(*self.q))
        slope, intercept, r_value, p_value, std_err = scipy.stats.linregress(simtimes_x, cputimes_y)
        self.drift[sample_time] = (slope / BASE_FREQ) * 100.

if len(sys.argv) != 2:
    print("compute evolution of drift over time")
    print("usage: %s drift-file" % (sys.argv[0],))
    sys.exit(1)

wnd = Window(60 * 1e6) # linear regression on 1 minute interval

f = open(sys.argv[1], 'r')
for line in f.readlines():
    line = line.strip(' ')
    if line == "" or line[0] == '#':
        continue
    fields = line.split(" ")

    try:
        simtime = int(fields[2])
        cputime = int(fields[3])
    except ValueError:
        print("cannot parse drift")
        print("line: '%s'" % (line,))
        sys.exit(1)

    wnd.insert(simtime, cputime)

print("# TIME(sec) DEV(0.0-1.0) PPM")
for simtime, drift in wnd.drift.items():
    simtime = float(simtime) / 1e6

    ppm = (1 - drift * 0.01) * 1e6

    print(simtime, drift, ppm)
