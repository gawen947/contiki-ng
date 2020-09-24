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

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

static struct uip_udp_conn *conn;

PROCESS(dst_node_process, "dst-node process");
AUTOSTART_PROCESSES(&dst_node_process);

static void tcpip_handler(void)
{
  if(uip_newdata()) {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    PRINTF("dst-node: rcv: '%s' from ", (char *)uip_appdata);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("\n");
  }
}

/* configure address, static routes and establish a connection to the sink */
PROCESS_THREAD(dst_node_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();

  PRINTF("dst-node: process started\n");

  /* configure address and routes */
  set_global_addr();
  set_iface_addr(&dst_addr);
  /* no route because we don't answer received packets */

  /* NETSTACK_RDC.off(1); */

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
