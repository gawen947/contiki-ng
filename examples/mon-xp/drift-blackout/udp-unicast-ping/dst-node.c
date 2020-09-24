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
#include <stdlib.h>
#include <stdint.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

static struct uip_udp_conn *conn;

PROCESS(dst_node_process, "Destination node");
AUTOSTART_PROCESSES(&dst_node_process);

static void tcpip_handler(void)
{
  uint16_t seqno;

  if(uip_newdata()) {
    seqno = *(uint16_t *)uip_appdata;
    monitor_record(MON_CT_CONTROL, MON_ENT_TEST, seqno);
  }
}

PROCESS_THREAD(dst_node_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();

  PRINTF("dst-node: process started\n");

  /* configure address and routes */
  set_global_addr();
  set_iface_addr(&dst_addr);
  /* no route because we don't answer received packets */

  /* create a UDP "connection" */
  conn = udp_new(&src_addr, UIP_HTONS(CLIENT_PORT), NULL);
  udp_bind(conn, UIP_HTONS(SERVER_PORT));

  /* show addresses to make sure that everything is correct */
  PRINTF("dst-node: UDP conn created\n");
  PRINTF("dst-node: iface = "); PRINT6ADDR(&dst_addr); PRINTF("\n");
  PRINTF("dst-node: dst   = "); PRINT6ADDR(&conn->ripaddr); /* should be src_addr */ PRINTF("\n");
  PRINTF("dst-node: local/remote port %u/%u\n", UIP_HTONS(conn->lport), UIP_HTONS(conn->rport));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event)
      tcpip_handler();
  }

  PROCESS_END();
}
