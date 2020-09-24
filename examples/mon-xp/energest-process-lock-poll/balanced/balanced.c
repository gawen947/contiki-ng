/*
 * Copyright (c) 2015, David Hauweele <david@hauweele.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include <stdio.h>

#include "contiki.h"
#include "mon/monitor.h"
#include "mon/context.h"
#include "mon/context/control.h"
#include "net/rime/rime.h"
#include "sys/energest.h"
#include "sys/rtimer.h"

#ifndef SEND_COUNT
# error "SEND_COUNT not defined => cannot compile!"
#endif

#ifndef CYCLES
# error "CYCLES not defined => cannot compile!"
#endif

static void recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  static int count = 0, i;
  static unsigned long last_current[ENERGEST_TYPE_MAX];

  if(count == 0) {
    /* mark the beginning of experiment */
    monitor_record(MON_CT_CONTROL, MON_ENT_TEST, 4);

    /* show legend */
    printf("LEGEND count rtimer_second cpu lpm irq transmit listen serial\n");
#ifdef RADIO_ISR_OFF
    printf("ISR_OFF enabled!\n");
#else
    printf("ISR OFF disabled!\n");
#endif /* RADIO_ISR_OFF */

    /* record current energy */
    energest_flush();
    for(i = 0; i < ENERGEST_TYPE_MAX; i++)
      last_current[i] = energest_total_time[i].current;
  }
  else if(count == (SEND_COUNT - 2)) {
    /* mark the end of experiment */
    monitor_record(MON_CT_CONTROL, MON_ENT_TEST, 5);

    /* record last energy */
    energest_flush();
    for(i = 0; i < ENERGEST_TYPE_MAX; i++)
      last_current[i] = energest_total_time[i].current - last_current[i];

    printf("ENERGY %d %u %lu %lu %lu %lu %lu %lu\n", count+1, RTIMER_SECOND,
           last_current[ENERGEST_TYPE_CPU],
           last_current[ENERGEST_TYPE_LPM],
           last_current[ENERGEST_TYPE_IRQ],
           last_current[ENERGEST_TYPE_TRANSMIT],
           last_current[ENERGEST_TYPE_LISTEN],
           last_current[ENERGEST_TYPE_SERIAL]);
  }

count++;
}

/*---------------------------------------------------------------------------*/
PROCESS(lock_poll_xp_balanced_process, "process-lock-poll XP (rime) balanced process");
AUTOSTART_PROCESSES(&lock_poll_xp_balanced_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(lock_poll_xp_balanced_process, ev, data)
{
  static struct broadcast_conn bc;
  static struct broadcast_callbacks bc_call = { recv };
  static volatile int n;

  PROCESS_BEGIN();

  broadcast_open(&bc, 129, &bc_call);

  /* create entity */
  /* we don't use the control events
     as we now the benchmarking code works well. */

  /* duty cycle of a fixed number of cycles
     each time this process has the hand. */
  while(1) {
    n = (CYCLES / 10);
    for(n = (CYCLES / 10) ; n ; n--);

    process_post(&lock_poll_xp_balanced_process, PROCESS_EVENT_CONTINUE, NULL);
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
