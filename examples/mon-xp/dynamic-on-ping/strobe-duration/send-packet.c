/* simplified version of send_packet() in contikimac */

static int send_packet(mac_callback_t mac_callback, void *mac_callback_ptr,
      struct rdc_buf_list *buf_list,
            int is_receiver_awake)
{
  t0 = RTIMER_NOW();
  seqno = packetbuf_attr(PACKETBUF_ATTR_MAC_SEQNO);
  for(strobes = 0, collisions = 0;
      got_strobe_ack == 0 && collisions == 0 &&
      RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + STROBE_TIME); strobes++) {

    /* is this for phase lock ? */
    if(!is_broadcast && (is_receiver_awake || is_known_receiver) &&
       !RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + MAX_PHASE_STROBE_TIME)) {
      PRINTF("miss to %d\n", packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[0]);
      break;
    }

    len = 0;

    {
      rtimer_clock_t wt;
#if WITH_PHASE_OPTIMIZATION
      rtimer_clock_t txtime = RTIMER_NOW();
#endif
#if RDC_CONF_HARDWARE_ACK
      int ret = NETSTACK_RADIO.transmit(transmit_len);
#else
      NETSTACK_RADIO.transmit(transmit_len);
#endif

#if RDC_CONF_HARDWARE_ACK
     /* For radios that block in the transmit routine and detect the
  ACK in hardware */
      if(ret == RADIO_TX_OK) {
        if(!is_broadcast) {
          got_strobe_ack = 1;
#if WITH_PHASE_OPTIMIZATION
          encounter_time = txtime;
#endif
          break;
        }
      } else if (ret == RADIO_TX_NOACK) {
      } else if (ret == RADIO_TX_COLLISION) {
          PRINTF("contikimac: collisions while sending\n");
          collisions++;
      }
      wt = RTIMER_NOW();
      while(RTIMER_CLOCK_LT(RTIMER_NOW(), wt + INTER_PACKET_INTERVAL)) { }
#else /* RDC_CONF_HARDWARE_ACK */
     /* Wait for the ACK packet */
      wt = RTIMER_NOW();
      while(RTIMER_CLOCK_LT(RTIMER_NOW(), wt + INTER_PACKET_INTERVAL)) { }

      if(!is_broadcast && (NETSTACK_RADIO.receiving_packet() ||
                           NETSTACK_RADIO.pending_packet() ||
                           NETSTACK_RADIO.channel_clear() == 0)) {
        uint8_t ackbuf[ACK_LEN];
        wt = RTIMER_NOW();
        while(RTIMER_CLOCK_LT(RTIMER_NOW(), wt + AFTER_ACK_DETECTECT_WAIT_TIME)) { }

        len = NETSTACK_RADIO.read(ackbuf, ACK_LEN);
        if(len == ACK_LEN && seqno == ackbuf[ACK_LEN - 1]) {
          got_strobe_ack = 1;
#if WITH_PHASE_OPTIMIZATION
          encounter_time = txtime;
#endif
          break;
        } else {
          PRINTF("contikimac: collisions while sending\n");
          collisions++;
        }
      }
#endif /* RDC_CONF_HARDWARE_ACK */
    }
  }

  off();

  PRINTF("contikimac: send (strobes=%u, len=%u, %s, %s), done\n", strobes,
         packetbuf_totlen(),
         got_strobe_ack ? "ack" : "no ack",
         collisions ? "collision" : "no collision");

#if CONTIKIMAC_CONF_COMPOWER
  /* Accumulate the power consumption for the packet transmission. */
  compower_accumulate(&current_packet);

  /* Convert the accumulated power consumption for the transmitted
     packet to packet attributes so that the higher levels can keep
     track of the amount of energy spent on transmitting the
     packet. */
  compower_attrconv(&current_packet);

  /* Clear the accumulated power consumption so that it is ready for
     the next packet. */
  compower_clear(&current_packet);
#endif /* CONTIKIMAC_CONF_COMPOWER */

  contikimac_is_on = contikimac_was_on;
  we_are_sending = 0;

  /* Determine the return value that we will return from the
     function. We must pass this value to the phase module before we
     return from the function.  */
  if(collisions > 0) {
    ret = MAC_TX_COLLISION;
  } else if(!is_broadcast && !got_strobe_ack) {
    ret = MAC_TX_NOACK;
  } else {
    ret = MAC_TX_OK;
  }

#if WITH_PHASE_OPTIMIZATION
  if(is_known_receiver && got_strobe_ack) {
    PRINTF("no miss %d wake-ups %d\n",
     packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[0],
           strobes);
  }

  if(!is_broadcast) {
    if(collisions == 0 && is_receiver_awake == 0) {
      phase_update(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
       encounter_time, ret);
    }
  }
#endif /* WITH_PHASE_OPTIMIZATION */

  return ret;
}
