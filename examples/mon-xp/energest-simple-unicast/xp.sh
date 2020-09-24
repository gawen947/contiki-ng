#!/bin/sh
#  Copyright (c) 2016, David Hauweele <david@hauweele.net>
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

# Exit on error
set -e

# The default target. You should probably not change that.
TARGET=sky

# Path to the tools directory.
TOOLS="../../../tools/"

# Fancies BSDs peculiarities.
case "$(uname -s)" in
FreeBSD)
  GNUMAKE=gmake
  ;;
*)
  GNUMAKE=make
  ;;
esac

if [ $# != 3 ]
then
  echo "usage: $0 TIME INITIAL_SEED NBR_RUN"
  echo "  TIME         Duration of the simulation (in seconds)."
  echo "  INITIAL_SEED Initial seed to use for the simulation (the seed for the first run)."
  echo "  NBR_RUN      Number of run per seed (with/without ISR opt)."

  exit 1
fi

cmd_time="$1"
cmd_seed="$2"
cmd_runs="$3"

rm -f command.info
echo "Time: $time" >  command.info
echo "Seed: $seed" >> command.info

temp_csc=$(mktemp)

clean() {
  # Makefile doesn't seems to clean correctly.
  # Don't know why so here's a little hack.
  firmware_path="$1"
  bin="$2"

  rm -rfv "$firmware_path"/obj_"$TARGET"
  rm -vf  "$firmware_path"/"$bin"
}

rebuild() {
  firmware_path="$1"

  echo "Rebuild $1..."

  o_pwd=$(pwd)

  cd "$firmware_path"

  # Touch some source files that
  # depends on environment variables.
  shift
  if [ $# -gt 0 ]
  then
    touch $*
  fi

  $GNUMAKE TARGET=$TARGET

  echo
  cd "$o_pwd"
}

clean_all() {
  clean with-monitor    fwd-node-monitor.sky
  clean without-monitor src-node.sky
  clean without-monitor fwd-node.sky
  clean without-monitor dst-node.sky
}

do_xp_run() {
  time="$1"
  seed="$2"

  time=$(rpnc "$time" 1000 .)


  if [ -n "$RADIO_ISR_OFF" ]
  then
    use_isroff=1
  else
    use_isroff=0
  fi

  out="results/seed=${seed}_time=${time}_isropt="$use_isroff".data"

  # Prepare the simulation file
  cat "simulation.template" | sed "s/##TIMEOUT##/$time/g" | sed "s/##SEED##/$seed/g" > "$temp_csc"

  # Clean old results. Ensure that we don't compute on a preceding XP.
  rm -f mspsim.trace mspsim.txt isr2poll.data isr2poll.stats isr-time.data isr-time.stats

  # Start the simulation.
  # This should generate the mspsim.trace
  echo "Starting simulation..."
  java -mx512m -jar "$TOOLS"/cooja/dist/cooja.jar -nogui="$temp_csc"
  echo

  # Parse the trace. The resulting file can be very large (~100MB)
  echo -n "Parsing resulting trace... "
  "$TOOLS"/monitor/trace mspsim.trace > mspsim.txt
  echo "done!"

  echo "Analysing trace: "

  echo -n "  Count events... "
  nb_isr=$(cat mspsim.txt | grep ISR | wc -l | tr -d ' ')
  nb_poll=$(cat mspsim.txt | grep POLL | wc -l | tr -d ' ')
  echo "done!"

  echo -n "  Generate intervals... "
  cat mspsim.txt | "$TOOLS"/monitor/scripts/isr2poll RADIO/CC2420/ISR RADIO/CC2420/DEVOFF >   isr2poll.data
  cat mspsim.txt | "$TOOLS"/monitor/scripts/isr2poll RADIO/CC2420/ISR RADIO/0001/0018  >   isr-time.data
  echo "done!"

  echo -n "  Compute statistics... "
  cat isr2poll.data | "$TOOLS"/monitor/scripts/bins 4 > isr2poll.stats
  cat isr-time.data | "$TOOLS"/monitor/scripts/bins 4 > isr-time.stats
  echo "done!"

  extract_stat() {
    cat "$1" | grep "^$2:" | cut -d':' -f2
  }
  extract_energy() {
    echo "$1" | cut -d' ' -f$2
  }
  delta_line() {
    rpnc $(echo "$1" | tail -n1) $(echo "$1" | head -n1) -
  }

  echo -n "  Extract stats... "
  # stats are extracted from isr2poll and named i2p
  # but it REALLY is ISR to OFF (see above)
  i2p_avg=$(extract_stat isr2poll.stats average)
  i2p_std=$(extract_stat isr2poll.stats stddev)
  i2p_5th=$(extract_stat isr2poll.stats 5th)
  i2p_95th=$(extract_stat isr2poll.stats 95th)
  i2p_sum=$(extract_stat isr2poll.stats sum)
  i2p_min=$(extract_stat isr2poll.stats min)
  i2p_max=$(extract_stat isr2poll.stats max)

  # Time we spend in ISR
  # Note: to compute this we need to modify the CC2420 ISR to generate a RADIO/0x1/0x18 event
  #       just before returning from the interruption.
  isr_avg=$(extract_stat isr-time.stats average)
  isr_std=$(extract_stat isr-time.stats stddev)
  isr_5th=$(extract_stat isr-time.stats 5th)
  isr_95th=$(extract_stat isr-time.stats 95th)
  isr_sum=$(extract_stat isr-time.stats sum)
  isr_min=$(extract_stat isr-time.stats min)
  isr_max=$(extract_stat isr-time.stats max)

  if cat COOJA.testlog | grep "ENERGY" > /dev/null
  then
    energy=$(cat COOJA.testlog | grep "ENERGY")
    e_count=$(extract_energy "$energy" 2)
    e_second=$(extract_energy "$energy" 3)
    e_cpu=$(extract_energy "$energy" 4)
    e_lpm=$(extract_energy "$energy" 5)
    e_irq=$(extract_energy "$energy" 6)
    e_transmit=$(extract_energy "$energy" 7)
    e_listen=$(extract_energy "$energy" 8)
    e_serial=$(extract_energy "$energy" 9)
  else
    e_count="NA"
    e_second="NA"
    e_cpu="NA"
    e_lpm="NA"
    e_irq="NA"
    e_transmit="NA"
    e_listen="NA"
    e_serial="NA"
  fi

  echo "done!"

  echo "Seed        : $seed" >>  "$out"
  echo "Time        : $(rpnc $time 1000 /) s" >> "$out"
  echo "Nb. ISR     : $nb_isr" >> "$out"
  echo "Nb. poll    : $nb_poll" >> "$out"
  echo >> "$out"
  echo "Time from radio ISR triggered to effective device radio OFF" >> "$out"
  echo "-----------------------------------------------------------" >> "$out"
  echo "ISR2DEVOFF avg.: $i2p_avg cyc." >> "$out"
  echo "ISR2DEVOFF std.: $i2p_std cyc." >> "$out"
  echo "ISR2DEVOFF 5th : $i2p_5th cyc." >> "$out"
  echo "ISR2DEVOFF 95th: $i2p_95th cyc." >> "$out"
  echo "ISR2DEVOFF min.: $i2p_min cyc." >> "$out"
  echo "ISR2DEVOFF max.: $i2p_max cyc." >> "$out"
  echo "ISR2DEVOFF sum.: $i2p_sum cyc." >> "$out"
  echo >> "$out"
  echo "Time spent in radio ISR" >> "$out"
  echo "-----------------------" >> "$out"
  echo "ISR avg.: $isr_avg cyc." >> "$out"
  echo "ISR std.: $isr_std cyc." >> "$out"
  echo "ISR 5th : $isr_5th cyc." >> "$out"
  echo "ISR 95th: $isr_95th cyc." >> "$out"
  echo "ISR min.: $isr_min cyc." >> "$out"
  echo "ISR max.: $isr_max cyc." >> "$out"
  echo "ISR sum.: $isr_sum cyc." >> "$out"
  echo >> "$out"
  echo "Energest" >> "$out"
  echo "-----------------------" >> "$out"
  echo "Count   : $e_count" >> "$out"
  echo "Second  : $e_second" >> "$out"
  echo "CPU     : $e_cpu" >> "$out"
  echo "LPM     : $e_lpm" >> "$out"
  echo "IRQ     : $e_irq" >> "$out"
  echo "TX      : $e_transmit" >> "$out"
  echo "RX      : $e_listen" >> "$out"
  echo "UART    : $e_serial" >> "$out"

  echo "$time $seed $use_isroff --- $nb_isr $nb_poll --- $i2p_avg $i2p_std $i2p_min $i2p_max $i2p_95th $i2p_sum" \
       "--- $isr_avg $isr_stdi --- $e_second --- $e_cpu $e_lpm $e_irq $e_transmit $e_listen $e_serial" >> "$runs_out"
}

runs_out="results/runs-output_start-seed=${cmd_seed}_time=${cmd_time}_runs=${cmd_runs}.data"

# legend
echo "# 1 : time" > "$runs_out"
echo "# 2 : seed" >> "$runs_out"
echo "# 3 : use ISR OFF opt." >> "$runs_out"
echo "# 4 : --- padding"  >> "$runs_out"
echo "# 5 : Nb. ISR"      >> "$runs_out"
echo "# 6 : Nb. poll"     >> "$runs_out"
echo "# 7 : --- padding"  >> "$runs_out"
echo "# 8 : ISR2DEVOFF avg." >> "$runs_out"
echo "# 9 : ISR2DEVOFF std." >> "$runs_out"
echo "# 10: ISR2DEVOFF min." >> "$runs_out"
echo "# 11: ISR2DEVOFF max." >> "$runs_out"
echo "# 12: ISR2DEVOFF 95th" >> "$runs_out"
echo "# 13: ISR2DEVOFF sum." >> "$runs_out"
echo "# 14: --- padding"  >> "$runs_out"
echo "# 15: ISR avg."     >> "$runs_out"
echo "# 16: ISR std."     >> "$runs_out" # expect this to be constant
echo "# 17: --- padding"  >> "$runs_out"
echo "# 18: duration in seconds" >> "$runs_out"
echo "# 19: --- padding"  >> "$runs_out"
echo "# 20: Energest CPU" >> "$runs_out"
echo "# 21: Energest LPM" >> "$runs_out"
echo "# 22: Energest IRQ" >> "$runs_out"
echo "# 23: Energest TX" >> "$runs_out"
echo "# 24: Energest RX" >> "$runs_out"
echo "# 25: Energest UART" >> "$runs_out"

prng() {
  # you wanted a linear congruential pseudo random number generator in shell?
  # now you got it!
  A=1103515245
  C=12345
  M=4294967296

  rpnc "$A" "$1" . "$C" +  "$M" mod
}

do_runs() {
  clean_all
  rebuild with-monitor
  rebuild without-monitor

  run_seed="$cmd_seed"
  for i in $(seq 1 "$cmd_runs")
  do
    do_xp_run "$cmd_time" "$run_seed"

    # compute new seed
    # we don't use increment because that would be too easy...
    run_seed=$(prng "$run_seed")
  done
}

unset RADIO_ISR_OFF
do_runs

export RADIO_ISR_OFF=true
do_runs

rm -f "$temp_csc"
#rm -f mspsim.trace mspsim.txt isr2poll.data isr2poll.stats isr-time.stats isr-time.data
#rm -f COOJA.log COOJA.testlog
unset RADIO_ISR_OFF
