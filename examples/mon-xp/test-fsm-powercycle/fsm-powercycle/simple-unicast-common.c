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

#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include "simple-unicast-common.h"

extern uip_ipaddr_t src_addr;
extern uip_ipaddr_t fwd_addr;
extern uip_ipaddr_t dst_addr;
static uip_ipaddr_t net_prefix;

static void print_local_addresses(void)
{
  int i;

  PRINTF("Client IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    uint8_t state = uip_ds6_if.addr_list[i].state;
    uint8_t type  = uip_ds6_if.addr_list[i].type;

    if(uip_ds6_if.addr_list[i].isused) {
      PRINTF("  ");
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);

      switch(state) {
      case ADDR_TENTATIVE:
        PRINTF(" (tentative/");
        break;
      case ADDR_PREFERRED:
        PRINTF(" (preferred/");
        break;
      case ADDR_DEPRECATED:
        PRINTF(" (deprecated/");
        break;
      default:
        PRINTF(" (unknown/");
        break;
      }

      switch(type) {
      case ADDR_ANYTYPE:
        PRINTF("any)");
        break;
      case ADDR_AUTOCONF:
        PRINTF("autoconf)");
        break;
      case ADDR_DHCP:
        PRINTF("dhcp)");
        break;
      case ADDR_MANUAL:
        PRINTF("manual)");
        break;
      default:
        PRINTF("unknown)");
        break;
      }

      PRINTF("\n");
    }
  }
}

void set_global_addr(void)
{
  uip_ip6addr(&src_addr, 0xaaaa, 0xbbbb, 0, 0, 0, 0, 0, 3);
  uip_ip6addr(&fwd_addr, 0xaaaa, 0xbbbb, 0, 0, 0, 0, 0, 2);
  uip_ip6addr(&dst_addr, 0xaaaa, 0xbbbb, 0, 0, 0, 0, 0, 1);
  uip_ip6addr(&net_prefix, 0xaaaa, 0xbbbb, 0, 0, 0, 0, 0, 0);
}

void set_iface_addr(uip_ipaddr_t *addr)
{
  uip_ds6_addr_add(addr, 0, ADDR_MANUAL);
  uip_ds6_prefix_add(&net_prefix, 64, 0,
                     0, 0, 0); /* last three arguments are neeeded
                                  because we compile with UIP_CONF_ROUTER */

  print_local_addresses();
}

void set_route(uip_ipaddr_t *to, uip_ipaddr_t *via)
{
  /* specific route to a destination */
  if(!uip_ds6_route_add(to, 128, via))
    PRINTF("error: cannot add route\n");
  else
    PRINTF("route added succesfully\n");
}

