/* Copyright (c) 2016, David Hauweele <david@hauweele.net>
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
#include "lib/random.h"
#include "net/netstack.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
#include "dev/serial-line.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "simple-unicast-common.h"

static struct uip_udp_conn *conn;

PROCESS(virtual_collect_process, "virtual-collect process");
PROCESS(src_node_process, "src-node process");
AUTOSTART_PROCESSES(&src_node_process, &virtual_collect_process);

static void virtual_collect_send(void)
{
  static const char msg[] = SEND_PAYLOAD;

  if(!conn) /* client not configured yet */
    return;

  uip_udp_packet_send(conn, &msg, sizeof(msg));
}

/* randomly send messages in <PERIOD seconds> + [O-<RANDWAIT seconds>]*/
PROCESS_THREAD(virtual_collect_process, ev, data)
{
  static struct etimer period_timer, wait_timer;

  PROCESS_BEGIN();

  etimer_set(&period_timer, CLOCK_SECOND * SEND_PERIOD);
  while(1) {
    PROCESS_WAIT_EVENT(); /* wait for the timer */

    if(ev == PROCESS_EVENT_TIMER) {
      if(data == &period_timer) {
        /* The first timer schedule the second random timer. */
        etimer_reset(&period_timer);
        etimer_set(&wait_timer, random_rand() % (CLOCK_SECOND * SEND_RANDOM_OFFSET));
      }
      else if (data == &wait_timer) {
        /* We are at PERIOD + RANDOM_OFFSET, we can now send the message. */
        virtual_collect_send();
      }
    }
  }

  PROCESS_END();
}

/* configure address, static routes and establish a connection to the sink */
PROCESS_THREAD(src_node_process, ev, data)
{
  static struct etimer wait_for_na;

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  PRINTF("src-node: process started\n");

  /* configure address */
  set_global_addr();
  set_iface_addr(&src_addr);

  /* send a NS for NH */
  uip_nd6_ns_output(&src_addr, NULL, &fwd_addr); /* get fwd lladdr */
  etimer_set(&wait_for_na, CLOCK_SECOND * 5); /* 5s to get the NA should be more than enough */
  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER && data == &wait_for_na)
      break;
  }
  set_route(&dst_addr, &fwd_addr);

  /* create a UDP "connection" */
  conn = udp_new(&dst_addr, UIP_HTONS(SERVER_PORT), NULL);
  udp_bind(conn, UIP_HTONS(CLIENT_PORT));

  /* show addresses to make sure that everything is correct */
  PRINTF("src-node: UDP conn created\n");
  PRINTF("src-node: iface = "); PRINT6ADDR(&src_addr); PRINTF("\n");
  PRINTF("src-node: dst   = "); PRINT6ADDR(&conn->ripaddr); /* should be dst_addr */ PRINTF("\n");
  PRINTF("src-node: via   = "); PRINT6ADDR(uip_ds6_route_nexthop(uip_ds6_route_lookup(&dst_addr))); PRINTF("\n");
  PRINTF("src-node: local/remote port %u/%u\n", UIP_HTONS(conn->lport), UIP_HTONS(conn->rport));

  while(1) {
    PROCESS_YIELD();
    /* ignore tcpip_event */
  }

  PROCESS_END();
}
