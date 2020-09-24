import sys

class ErgCst(object):
    def __init__(self, current, volt):
        self.current = current
        self.volt    = volt

    def energy(self, energest_value, rtimer_sec, runtime):
        # convert from mW to uW
        return (energest_value * self.current * self.volt) / (rtimer_sec * runtime)

    def __str__(self):
        return "%3.1f mA @%3.1f V" % (self.current, self.volt)

# characteristics extracted
# from the TMoteSky datasheet
# power are given in mA and voltage in V
MOTE="TMote Sky"
TRANSCEIVER="CC2420"
CPU="MSP430F16xx"
SOURCE="TMoteSky datasheet p4 typical operating conditions"
PARAM = { "transmit" : ErgCst(19.5, 3.3),   # mcu ON, radio TX
          "listen"   : ErgCst(21.8, 3.3),   # mcu ON, radio RX
          "cpu"      : ErgCst(1.8, 3.3),    # mcu ON, radio OFF
          "lpm"      : ErgCst(0.0545, 3.3), # mcu IDLE
          "irq"      : ErgCst(1.8, 3.3),    # same as CPU
          "serial"   : ErgCst(1., 1.) }     # I've no idea...

print "# Power are shown in mW"
print "#"
print "# Source: %s" % (SOURCE,)
print "#"
print "# MOTE: %s" % (MOTE,)
print "# CPU: %s" % (CPU,)
print "# TRANSCEIVER: %s" % (TRANSCEIVER,)
print "#"
print "# Value:"
for k,v in PARAM.iteritems():
    print "# %s: %s" % (k, v)
print "#"
print "# extra legend:"
print "# 9: is actually XP duration (s)"
print "# 10: is actually XP duration (cyc)"
print "# 11: is actually Count (message sent)"
print "# 19-23: are converted from cycles to seconds"
print "# 21: field is power expressed in mW"
print "# NOTE that the runtime is now displayed in SECONDS and energest values in MILLIWATTS"
print "# NOTE erroneous values are ignored"
print "#"

f = open(sys.argv[1], "r")
for line in f:
    # comments displayed as is
    line = line.strip()
    if line == "" or line[0] == "#":
        print line
        continue

    line = line.split(' ')

    for i in xrange(9,24):
        try:
            line[i] = float(line[i])
        except:
            line[i] = "NA"

    rtimer_sec = line[12]
    runtime    = line[9] / 1000 # we want this in seconds
    line[9]    = runtime

    for idx,name in ((13, "cpu"),
                     (14, "lpm"),
                     (15, "irq"),
                     (16, "transmit"),
                     (17, "listen"),
                     (18, "serial")):
        if runtime > 0:
            line[idx] = PARAM[name].energy(line[idx], rtimer_sec, runtime)
        else:
            line[idx] = "NA"

    if runtime <= 0:
        continue

    line = map(str, line)

    print ' '.join(line)
