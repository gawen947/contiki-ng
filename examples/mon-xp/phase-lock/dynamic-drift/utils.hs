{-
  Utils Haskell functions for the experiment.
-}

-- Convert to ppm and back.
dev2ppm dev = (1 - dev) * 1e6
ppm2dev ppm = 1 - (ppm / 1e6)


-- Predict observed deviation with requested deviation and alpha factor.
modelDev dev alpha = dev * (alpha + 1) / (dev * alpha + 1)

-- Alpha factor for various values of the sleep parameter.
alpha32  = 50.990796
alpha64  = 101.259515
alpha128 = 193.570661

-- Convert a value of a sleep timer into millisecs
timer2ms counterValue = 1e3 * counterValue / 32768

-- Drift blackout periodicity and duration
--t_c = 500e-6
--realT_i t2t t_s = t2t - t_s
ppm2hz ppm = ppm / 1e6
blackoutDuration t_c t_i ppm = (t_i - t_c) / (ppm2hz ppm)
blackoutPeriod   t_s t_i ppm = (t_s + t_i) / (ppm2hz ppm)
blackoutInfo     t_s t_i t_c ppm = (blackoutDuration t_c t_i ppm, blackoutPeriod t_s t_i ppm)

-- measured value for IPI=1ms, CST=def/2, payload=37B in COOJA
t_s=3860e-6 -- +/- 53.33 µs
t_i=1070.45e-6 -- +/- 3.27 µs
t_c=636e-6 -- +/- 0 µs or 0.22 µs
