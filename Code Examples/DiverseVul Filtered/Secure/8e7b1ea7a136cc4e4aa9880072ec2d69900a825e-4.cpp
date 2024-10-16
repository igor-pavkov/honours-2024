void ndpi_search_openvpn(struct ndpi_detection_module_struct* ndpi_struct,
                         struct ndpi_flow_struct* flow) {
  struct ndpi_packet_struct* packet = &flow->packet;
  const u_int8_t * ovpn_payload = packet->payload;
  const u_int8_t * session_remote;
  u_int8_t opcode;
  u_int8_t alen;
  int8_t hmac_size;
  int8_t failed = 0;
  /* No u_ */int16_t ovpn_payload_len = packet->payload_packet_len;
  
  if(ovpn_payload_len >= 40) {
    // skip openvpn TCP transport packet size
    if(packet->tcp != NULL)
      ovpn_payload += 2, ovpn_payload_len -= 2;;

    opcode = ovpn_payload[0] & P_OPCODE_MASK;

    if(packet->udp) {
#ifdef DEBUG
      printf("[packet_id: %u][opcode: %u][Packet ID: %d][%u <-> %u][len: %u]\n",
	     flow->num_processed_pkts,
	     opcode, check_pkid_and_detect_hmac_size(ovpn_payload),
	     htons(packet->udp->source), htons(packet->udp->dest), ovpn_payload_len);	   
#endif
      
      if(
	 (flow->num_processed_pkts == 1)
	 && (
	     ((ovpn_payload_len == 112)
	      && ((opcode == 168) || (opcode == 192))
	      )
	     || ((ovpn_payload_len == 80)
		 && ((opcode == 184) || (opcode == 88) || (opcode == 160) || (opcode == 168) || (opcode == 200)))
	     )) {
	NDPI_LOG_INFO(ndpi_struct,"found openvpn\n");
	ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_OPENVPN, NDPI_PROTOCOL_UNKNOWN);
	return;
      }
    }
    
    if(flow->ovpn_counter < P_HARD_RESET_CLIENT_MAX_COUNT && (opcode == P_CONTROL_HARD_RESET_CLIENT_V1 ||
				    opcode == P_CONTROL_HARD_RESET_CLIENT_V2)) {
      if(check_pkid_and_detect_hmac_size(ovpn_payload) > 0) {
        memcpy(flow->ovpn_session_id, ovpn_payload+1, 8);

        NDPI_LOG_DBG2(ndpi_struct,
		 "session key: %02x%02x%02x%02x%02x%02x%02x%02x\n",
		 flow->ovpn_session_id[0], flow->ovpn_session_id[1], flow->ovpn_session_id[2], flow->ovpn_session_id[3],
		 flow->ovpn_session_id[4], flow->ovpn_session_id[5], flow->ovpn_session_id[6], flow->ovpn_session_id[7]);
      }
    } else if(flow->ovpn_counter >= 1 && flow->ovpn_counter <= P_HARD_RESET_CLIENT_MAX_COUNT &&
            (opcode == P_CONTROL_HARD_RESET_SERVER_V1 || opcode == P_CONTROL_HARD_RESET_SERVER_V2)) {

      hmac_size = check_pkid_and_detect_hmac_size(ovpn_payload);

      if(hmac_size > 0) {
	u_int16_t offset = P_PACKET_ID_ARRAY_LEN_OFFSET(hmac_size);
	  
        alen = ovpn_payload[offset];
	
        if (alen > 0) {
	  offset += 1 + alen * 4;

	  if((offset+8) <= ovpn_payload_len) {
	    session_remote = &ovpn_payload[offset];
	    
	    if(memcmp(flow->ovpn_session_id, session_remote, 8) == 0) {
	      NDPI_LOG_INFO(ndpi_struct,"found openvpn\n");
	      ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_OPENVPN, NDPI_PROTOCOL_UNKNOWN);
	      return;
	    } else {
	      NDPI_LOG_DBG2(ndpi_struct,
			    "key mismatch: %02x%02x%02x%02x%02x%02x%02x%02x\n",
			    session_remote[0], session_remote[1], session_remote[2], session_remote[3],
			    session_remote[4], session_remote[5], session_remote[6], session_remote[7]);
	      failed = 1;
	    }
	  } else
	    failed = 1;
	} else
          failed = 1;
      } else
        failed = 1;
    } else
      failed = 1;

    flow->ovpn_counter++;
    
    if(failed) {
      NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
    }
  }
}