import sys

rtimer_sec=float(raw_input("RTIMER_SECOND ? "))
runtime=float(raw_input("XP duration (for Energest) ? "))

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

while True:
    print "---"
    for k,v in PARAM.iteritems():
        energest=float(raw_input("%s energest value ? " % (k,)))
        print "%s: %3.3f mW" % (k, v.energy(energest, rtimer_sec, runtime))
