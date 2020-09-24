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

#include "mon-names.h"


/* Includes IDs directly from Contiki. */
#include "../../core/mon/states.h"
#include "../../core/mon/context.h"
#include "../../core/mon/context/control.h"
#include "../../core/mon/context/mon-ct-radio.h"
#include "../../core/mon/context/mon-ct-powercycle.h"
#include "../../core/mon/context/mon-xp.h"

/* Macro to ease registering context, state, entities names. */
#define REG_CTX(context) reg_context_name(MON_CT_ ## context, #context)
#define REG_ST(context, state) reg_state_name(context, MON_ST_ ## state, #state)
#define REG_ENT(context, entity) reg_entity_name(context, MON_ENT_ ## entity, #entity)
#define REG_COMMON_ST(state) reg_common_state_name(MON_ST_ ## state, #state)
#define REG_COMMON_ENT(entity) reg_common_entity_name(MON_ENT_ ## entity, #entity)

#define REG_RADIO_ST(state) reg_state_name(MON_CT_RADIO, MON_ST_RADIO_ ## state, #state)
#define REG_POWERCYCLE_ST(state) reg_state_name(MON_CT_POWERCYCLE, MON_ST_POWERCYCLE_ ## state, #state)
#define REG_XP_ST(state) reg_state_name(MON_CT_CONTROL, MON_XP_ ## state, #state)

static void register_mon_ids_radio(void)
{
  /* Register radio states names. */
  REG_RADIO_ST(ON);
  REG_RADIO_ST(OFF);
  REG_RADIO_ST(CCA);
  REG_RADIO_ST(ISR);
  REG_RADIO_ST(POLL);
  REG_RADIO_ST(RDC);
  REG_RADIO_ST(READ);
  REG_RADIO_ST(PREPARE);
  REG_RADIO_ST(TRANSMIT);
  REG_RADIO_ST(DEVOFF);
  REG_RADIO_ST(CCA_END);
  REG_RADIO_ST(CCA_FAIL);

  /* Register radio entities names. */
  REG_ENT(MON_CT_RADIO, CC2420);
}

static void register_mon_ids_powercycle(void)
{
  /* Register powercycle states names. */
  REG_POWERCYCLE_ST(RADIO_ON_1);
  REG_POWERCYCLE_ST(RADIO_OFF_1);
  REG_POWERCYCLE_ST(RADIO_OFF_2);
  REG_POWERCYCLE_ST(RADIO_OFF_3);
  REG_POWERCYCLE_ST(RADIO_OFF_4);
  REG_POWERCYCLE_ST(RADIO_CCA_1);
  REG_POWERCYCLE_ST(RADIO_CCA_2);
  REG_POWERCYCLE_ST(LISTEN_TIME_AFTER_PACKET_DETECTED);
  REG_POWERCYCLE_ST(MAX_SILENCE);
  REG_POWERCYCLE_ST(FASTSLEEP);
  REG_POWERCYCLE_ST(PACKET_SEEN);
  REG_POWERCYCLE_ST(SILENCE);
  REG_POWERCYCLE_ST(ACTIVITY);
  REG_POWERCYCLE_ST(RECEIVING);
  REG_POWERCYCLE_ST(PENDING);
  REG_POWERCYCLE_ST(NEXT_CCA);
  REG_POWERCYCLE_ST(NEXT_CCA_CHECK);
  REG_POWERCYCLE_ST(NEXT_POWERCYCLE);
  REG_POWERCYCLE_ST(ARCH_SLEEP);

  REG_POWERCYCLE_ST(FSM_START); /* FSM powercycle */
  REG_POWERCYCLE_ST(FSM_PROBES);
  REG_POWERCYCLE_ST(FSM_PROBE);
  REG_POWERCYCLE_ST(FSM_SEEN);
  REG_POWERCYCLE_ST(FSM_ACTIVITY_CHECK);
  REG_POWERCYCLE_ST(FSM_PERIODS_CHECK);
  REG_POWERCYCLE_ST(FSM_PENDING);
  REG_POWERCYCLE_ST(FSM_SLEEP_END);
  REG_POWERCYCLE_ST(FSM_END);

  /* Register powercycle entities names. */
  REG_ENT(MON_CT_POWERCYCLE, CONTIKIMAC);
}

static void register_mon_ids_control(void)
{
  /* Register control entities names. */
  REG_ENT(MON_CT_CONTROL, CAL); /* MON_ENT_CAL */
  REG_ENT(MON_CT_CONTROL, TEST);
  REG_ENT(MON_CT_CONTROL, XP);

  /* Register XP states names. */
  REG_XP_ST(ACLK);
  REG_XP_ST(MCLK);
  REG_XP_ST(BLINK);
  REG_XP_ST(LFXT1OF);
  REG_XP_ST(OFIFG);
  REG_XP_ST(NOSLEEP);

  /* Register control states names. */
  REG_ST(MON_CT_CONTROL, CHECK); /* MON_ST_CHECK */
}

void register_mon_ids(void)
{
  /* Common state/entity names. */
  REG_COMMON_ST(CREATE); /* MON_ST_CREATE */
  REG_COMMON_ST(DESTROY);
  REG_COMMON_ST(INFO);

  /* Register context names. */
  REG_CTX(CONTROL); /* MON_CT_CONTROL */
  REG_CTX(SCHED);
  REG_CTX(NETIN);
  REG_CTX(NETOUT);
  REG_CTX(CPU);
  REG_CTX(RADIO);
  REG_CTX(POWERCYCLE);

  register_mon_ids_control();
  register_mon_ids_radio();
  register_mon_ids_powercycle();
}
