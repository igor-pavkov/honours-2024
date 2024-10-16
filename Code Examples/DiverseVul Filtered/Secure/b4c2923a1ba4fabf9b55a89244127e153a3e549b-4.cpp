const unsigned char * r_jwe_get_payload(jwe_t * jwe, size_t * payload_len) {
  if (jwe != NULL) {
    if (payload_len != NULL) {
      *payload_len = jwe->payload_len;
    }
    return jwe->payload;
  }
  return NULL;
}