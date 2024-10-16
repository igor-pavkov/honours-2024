int r_jwe_set_payload(jwe_t * jwe, const unsigned char * payload, size_t payload_len) {
  int ret;

  if (jwe != NULL) {
    o_free(jwe->payload);
    if (payload != NULL && payload_len) {
      if ((jwe->payload = o_malloc(payload_len)) != NULL) {
        memcpy(jwe->payload, payload, payload_len);
        jwe->payload_len = payload_len;
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_payload - Error allocating resources for payload");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jwe->payload = NULL;
      jwe->payload_len = 0;
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}