int8_t check_pkid_and_detect_hmac_size(const u_int8_t * payload) {
  // try to guess
  if(get_packet_id(payload, P_HMAC_160) == 1)
    return P_HMAC_160;
  
  if(get_packet_id(payload, P_HMAC_128) == 1)    
    return P_HMAC_128;
  
  return(-1);
}