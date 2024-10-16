int r_jwe_set_iv(jwe_t * jwe, const unsigned char * iv, size_t iv_len) {
  int ret;
  unsigned char * iv_b64 = NULL;
  size_t iv_b64_len = 0;

  if (jwe != NULL) {
    o_free(jwe->iv);
    if (iv != NULL && iv_len) {
      if ((jwe->iv = o_malloc(iv_len)) != NULL) {
        memcpy(jwe->iv, iv, iv_len);
        jwe->iv_len = iv_len;
        if ((iv_b64 = o_malloc(jwe->iv_len*2)) != NULL) {
          if (o_base64url_encode(jwe->iv, jwe->iv_len, iv_b64, &iv_b64_len)) {
            o_free(jwe->iv_b64url);
            jwe->iv_b64url = (unsigned char *)o_strndup((const char *)iv_b64, iv_b64_len);
            ret = RHN_OK;
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_iv - Error o_base64url_encode iv_b64");
            ret = RHN_ERROR;
          }
          o_free(iv_b64);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_iv - Error allocating resources for iv_b64");
          ret = RHN_ERROR_MEMORY;
        }
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_iv - Error allocating resources for iv");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jwe->iv = NULL;
      jwe->iv_len = 0;
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}