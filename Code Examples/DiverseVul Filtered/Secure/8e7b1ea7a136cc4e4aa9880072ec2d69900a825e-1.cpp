u_int32_t get_packet_id(const u_int8_t * payload, u_int8_t hms) {
  return(ntohl(*(u_int32_t*)(payload + P_HARD_RESET_PACKET_ID_OFFSET(hms))));
}