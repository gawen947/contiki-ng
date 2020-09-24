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

# Shall we clean  the targets initially?
PRE_CLEAN=true

# Shall we clean the targets each time?
CLEAN_BUILD=false

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

if [ $# != 5 ]
then
  echo "usage: $0 MIN_CYCLES STEP MAX_CYCLES STEP SEND_COUNT TIME"
  echo "  MIN_CYCLES Mininmum number of working cycles."
  echo "  STEP       Step in number of working cyclces between each run of the simulation."
  echo "  MAX_CYCLES Maximum number of working cycles."
  echo "  SEND_COUNT Number of time the probe messages should be sent on the air."
  echo "  TIME       Duration of the simulation. Must be choosen accordingly to SEND_COUNT."

  exit 1
fi

min_cycles="$1"
step="$2"
max_cycles="$3"
send_count="$4"
time="$5"

rm -f command.info
echo "Min : $min_cycles" > command.info
echo "Step: $step" >> command.info
echo "Max : $max_cycles" >> command.info
echo "Cnt : $send_count" >> command.info
echo "Time: $time" >> command.info

time=$(rpnc "$time" 1000 .)

temp_csc=$(mktemp)

clean() {
  # Makefile doesn't seems to clean correctly.
  # Don't know why so here's a little hack.
  firmware_path="$1"
  bin="$2"

  rm -rfv "$firmware_path"/obj_"$TARGET"
  rm -vf "$firmware_path"/"$bin"
}

rebuild() {
  firmware_path="$1"

  echo "Rebuild $1..."

  o_pwd=$(pwd)

  cd "$firmware_path"

  # Touch some source files that
  # depends on environment variables.
  shift
  touch $*

  $GNUMAKE TARGET=$TARGET

  echo
  cd "$o_pwd"
}

# Legend
rm -f isr2poll_stats.data control_stats.data
echo "# 0: Workload (cyc.)" > isr2poll_stats.data
echo "# 1: Nb. ISR" >> isr2poll_stats.data
echo "# 2: Nb. POLL" >> isr2poll_stats.data
echo "# 3: Average" >> isr2poll_stats.data
echo "# 4: StdDev" >> isr2poll_stats.data
echo "# 5: 5th ptile" >> isr2poll_stats.data
echo "# 6: 95th ptile" >> isr2poll_stats.data
echo "# 7: Min" >> isr2poll_stats.data
echo "# 8: Max" >> isr2poll_stats.data
echo "# 9: XP duration (ms)" >> isr2poll_stats.data
echo "# 10: XP duration (cyc.)" >> isr2poll_stats.data
echo "# 11: Count (message sent)" >> isr2poll_stats.data
echo "# 12: RTIMER_SECOND" >> isr2poll_stats.data
echo "# 13: Current Time: CPU" >> isr2poll_stats.data
echo "# 14: Current Time: LPM" >> isr2poll_stats.data
echo "# 15: Current Time: IRQ" >> isr2poll_stats.data
echo "# 16: Current Time: TRANSMIT" >> isr2poll_stats.data
echo "# 17: Current Time: LISTEN" >> isr2poll_stats.data
echo "# 18: Current Time: SERIAL" >> isr2poll_stats.data
echo "#" >> isr2poll_stats.data
echo "# 19: ISR avg (cyc.)" >> isr2poll_stats.data
echo "# 20: ISR std (cyc.)" >> isr2poll_stats.data
echo "# 21: ISR sum (cyc.)" >> isr2poll_stats.data
echo "# 22: ISR min (cyc.)" >> isr2poll_stats.data
echo "# 23: ISR max (cyc.)" >> isr2poll_stats.data
echo "#" >> isr2poll_stats.data
if [ -n "$RADIO_ISR_OFF" ]
then
  echo "# ISR OFF enabled!" >> isr2poll_stats.data
else
  echo "# ISR OFF disabled! (default MAC)" >> isr2poll_stats.data
fi
cp isr2poll_stats.data control_stats.data

if $PRE_CLEAN
then
  clean balanced balanced.sky
  clean ping     ping.sky
fi

export SEND_COUNT="$send_count"
rebuild ping ping.c

# Prepare the simulation file
cat "simulation.template" | sed "s/##TIMEOUT##/$time/g" > "$temp_csc"

for cycles in $(seq "$min_cycles" "$step" "$max_cycles")
do
  # Clean old results. Ensure that we don't compute on a preceding XP.
  rm -f mspsim.trace mspsim.txt isr2poll.data isr2poll.stats control.data control.stats

  # Some info about the current run.
  echo "-------------------------"
  echo "Run $cycles / $max_cycles"
  if [ "$min_cycles" != "$max_cycles" ]
  then
    printf "Progress %3.2f%%\n" $(rpnc "$cycles" "$min_cycles" - "$max_cycles" "$min_cycles" - / 100 .)
  fi
  echo

  # Clean if needed.
  if $CLEAN_BUILD
  then
    clean balanced balanced.sky
  fi

  # Compile the balanced application with new workload.
  export CYCLES="$cycles"
  rebuild balanced balanced.c

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
  cat mspsim.txt | "$TOOLS"/monitor/scripts/isr2poll RADIO/CC2420/ISR RADIO/CC2420/OFF > isr2poll.data
  cat mspsim.txt | "$TOOLS"/monitor/scripts/isr2poll RADIO/CC2420/ISR RADIO/0001/0018  > isr-time.data
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
  i2p_avg=$(extract_stat isr2poll.stats average)
  i2p_std=$(extract_stat isr2poll.stats stddev)
  i2p_5th=$(extract_stat isr2poll.stats 5th)
  i2p_95th=$(extract_stat isr2poll.stats 95th)
  i2p_min=$(extract_stat isr2poll.stats min)
  i2p_max=$(extract_stat isr2poll.stats max)

  # Time in cyc. we spend in ISR
  # Note: to compute this we need to modify the CC2420 ISR to generate a RADIO/0x1/0x18 event
  #       just before returning.
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

  xp_time_ms=$(cat mspsim.txt | grep ENT=TEST | grep -oE "NODE_TIME_MS=[0-9\.]*" | cut -d'=' -f2)
  xp_time_cyc=$(cat mspsim.txt | grep ENT=TEST | grep -oE "NODE_CYCLES=[0-9\.]*" | cut -d'=' -f2)

  xp_delta_ms=$(delta_line "$xp_time_ms")
  xp_delta_cyc=$(delta_line "$xp_time_cyc")
  echo "done!"

  echo "$cycles $nb_isr $nb_poll $i2p_avg $i2p_std $i2p_5th $i2p_95th $i2p_min $i2p_max $xp_delta_ms $xp_delta_cyc $e_count $e_second $e_cpu $e_lpm $e_irq $e_transmit $e_listen $e_serial $isr_avg $isr_std $isr_sum $isr_min $isr_max" >> isr2poll_stats.data
done

rm -f "$temp_csc"
#rm -f mspsim.trace mspsim.txt isr2poll.data isr2poll.stats control.data control.stats isr-time.data isr-time.stats
#rm -f COOJA.log COOJA.testlog
