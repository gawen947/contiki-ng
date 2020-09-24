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

#ifndef MON_CT_RADIO_H_
#define MON_CT_RADIO_H_

enum {
  MON_ST_RADIO_ON,        /* driver was requested to turn on the radio */
  MON_ST_RADIO_OFF,       /* driver was requested to shut down the radio */
  MON_ST_RADIO_CCA,       /* CCA requested */
  MON_ST_RADIO_ISR,       /* ISR started in radio driver */
  MON_ST_RADIO_POLL,      /* radio process polled from ISR */
  MON_ST_RADIO_RDC,       /* NETSTACK_RDC.input() */
  MON_ST_RADIO_READ,      /* read (ACK) from the RDC layer */
  MON_ST_RADIO_PREPARE,   /* prepare packet in transceiver memory */
  MON_ST_RADIO_TRANSMIT,  /* transmit the packet */
  MON_ST_RADIO_DEVOFF,    /* radio driver effectively shuts down the radio */
  MON_ST_RADIO_CCA_END,   /* CCA request end */
  MON_ST_RADIO_CCA_FAIL,  /* Failure to issue a CCA (because radio is locked) */
};

enum {
  MON_ENT_CC2420
};

#endif /* MON_CT_RADIO_H_ */
