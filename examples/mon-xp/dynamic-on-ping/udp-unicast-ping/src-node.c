/* Copyright (c) 2019, David Hauweele <david@hauweele.net>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"

#include "mon/monitor.h"
#include "mon/context.h"
#include "mon/context/control.h"
#include "mon/context/mon-xp.h"

#include "simple-unicast-common.h"

#include <stdio.h>
#include <stdint.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

static struct uip_udp_conn *conn;
static rtimer_clock_t deadline;

PROCESS(src_node_process, "Source node");
AUTOSTART_PROCESSES(&src_node_process);

static void send_msg(void)
{
  static int size;
  static uint16_t seqno;
  static char payload[127]; /* FIXME: too large */

  *(uint16_t *)payload = seqno;

  /* We are ~0.2s + 'phase-mode-delay' early. Spinlock to reach the deadline. */
  while(RTIMER_CLOCK_LT(RTIMER_NOW(), deadline));
  deadline += RTIMER_SECOND;

  monitor_record(MON_CT_CONTROL, MON_ENT_TEST, seqno);
  uip_udp_packet_send(conn, &payload, 2 + PADDING);

  seqno++;
}

PROCESS_THREAD(src_node_process, ev, data)
{
  static struct etimer et;
  static struct etimer wait_for_na;

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  PRINTF("src-node: process started\n");

  /* configure address */
  set_global_addr();
  set_iface_addr(&src_addr);

  /* send a NS for neighbor */
  uip_nd6_ns_output(&src_addr, NULL, &dst_addr); /* get dst lladdr */
  etimer_set(&wait_for_na, CLOCK_SECOND * 5); /* 5s to get the NA should be more than enough */
  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER && data == &wait_for_na)
      break;
  }
  /* no route to add here but see energest-simple-unicast/without-monitor
     for an example of forwarded unicast without RPL. */

  /* create a UDP "connection" */
  conn = udp_new(&dst_addr, UIP_HTONS(SERVER_PORT), NULL);
  udp_bind(conn, UIP_HTONS(CLIENT_PORT));

  /* show addresses to make sure that everything is correct */
  PRINTF("src-node: UDP conn created\n");
  PRINTF("src-node: iface = "); PRINT6ADDR(&src_addr); PRINTF("\n");
  PRINTF("src-node: dst   = "); PRINT6ADDR(&conn->ripaddr); /* should be dst_addr */ PRINTF("\n");
  PRINTF("src-node: local/remote port %u/%u\n", UIP_HTONS(conn->lport), UIP_HTONS(conn->rport));

  deadline = RTIMER_NOW() + RTIMER_SECOND;
  while(1) {
    /* 1 second delay to match Marie-Paule's thesis. */
    etimer_set(&et, 0.8 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    send_msg();
  }

  PROCESS_END();
}
