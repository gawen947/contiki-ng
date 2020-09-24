import math

import cooja

class LegacyCoojaMote(cooja.Mote):
    """
    Classical mote execution with deviation
    parameter as found in Cooja.
    """

    def __init__(self, simulation, mspSim, ID):
        cooja.Mote.__init__(self, simulation, mspSim, ID)
        self.deviation = 1.0
        self.total     = 0
        self.executed  = 0
        self.old_t     = 0

    def setDeviation(self, deviation):
        self.deviation = deviation

    def moteExecute(self, t, duration):
        if duration != 1:
            raise Exception("except duration of 1 for simple model")

        jump = t - self.old_t
        original_jump = jump

        if self.total == 0:
            ptotal = -1
        else:
            ptotal = self.total
        #print(jump,  self.executed, self.total, self.deviation * self.total, float(self.executed) / ptotal)
        if self.deviation * self.total < self.executed:
            """
            We executed too much frames.
            We must skip the next frame
            to reach the requested ratio.
            """
            jump -= self.executed - self.deviation * self.total
            print self.deviation * self.total, self.executed, original_jump, jump
            assert jump > 0

            # scheduler only takes the nearest scheduled
            # execution into account.
            self.simulation.scheduleNextExec(t + 1, self)
        self.executed += jump
        self.total    += original_jump

        new_t = self.mspsim.stepOneMicro(jump, 1) + t + 1
        self.simulation.scheduleNextExec(new_t, self)
        self.old_t = t


    def moteInterrupt(self, t, irq):
        self.mspsim.interrupt(irq)

class NewdriftCoojaMote(cooja.Mote):
    """
    Same mote as Cooja but with a new
    implementation of the drift parameter.
    """

    def __init__(self, simulation, mspSim, ID):
        cooja.Mote.__init__(self, simulation, mspSim, ID)
        self.deviation         = 1.0
        self.invDeviation      = 1.0 / self.deviation
        self.old_t             = 0
        self.jumpError         = 0.
        self.executeDeltaError = 0.

    def setDeviation(self, deviation):
        self.deviation = deviation

    def moteExecute(self, t, duration):
        jump = t - self.old_t
        exactJump = jump * self.deviation
        jump      = int(math.floor(exactJump))

        self.jumpError += exactJump - jump

        if self.jumpError > 0.5:
            jump += 1
            self.jumpError -= 1.0

        executeDelta = self.mspsim.stepOneMicro(jump, duration) + duration

        exactExecuteDelta = executeDelta * self.invDeviation
        executeDelta = int(math.floor(exactExecuteDelta))

        self.executeDeltaError += exactExecuteDelta - executeDelta

        if self.executeDeltaError > 0.5:
            executeDelta += 1
            self.executeDeltaError -= 1.0

        self.simulation.scheduleNextExec(t + executeDelta, self)
        self.old_t = t

    def moteInterrupt(self, t, irq):
        self.mspsim.interrupt(irq)
