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
#include "dev/serial-line.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "simple-unicast-common.h"

PROCESS(fwd_node_process, "fwd-node process");
AUTOSTART_PROCESSES(&fwd_node_process);

/* configure address, static routes and then do nothing */
PROCESS_THREAD(fwd_node_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();

  PRINTF("fwd-node: process started\n");

  /* configure address and routes */
  set_global_addr();
  set_iface_addr(&fwd_addr);
  /* no route configured => all nodes are direct neighbors */

  PRINTF("fwd-node: iface = "); PRINT6ADDR(&fwd_addr); PRINTF("\n");

  while(1) {
    PROCESS_YIELD();
    /* ignore tcpip_event */
  }

  PROCESS_END();
}
