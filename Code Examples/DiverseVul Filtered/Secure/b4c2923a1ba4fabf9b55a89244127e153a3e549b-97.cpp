int r_jwe_set_aad(jwe_t * jwe, const unsigned char * aad, size_t aad_len) {
  int ret;
  unsigned char * aad_b64 = NULL;
  size_t aad_b64_len = 0;

  if (jwe != NULL) {
    o_free(jwe->aad_b64url);
    jwe->aad_b64url = NULL;
    o_free(jwe->aad);
    if (aad != NULL && aad_len) {
      if ((jwe->aad = o_malloc(aad_len)) != NULL) {
        memcpy(jwe->aad, aad, aad_len);
        jwe->aad_len = aad_len;
        if ((aad_b64 = o_malloc(jwe->aad_len*2)) != NULL) {
          if (o_base64url_encode(jwe->aad, jwe->aad_len, aad_b64, &aad_b64_len)) {
            o_free(jwe->aad_b64url);
            jwe->aad_b64url = (unsigned char *)o_strndup((const char *)aad_b64, aad_b64_len);
            ret = RHN_OK;
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_aad - Error o_base64url_encode aad_b64");
            ret = RHN_ERROR;
          }
          o_free(aad_b64);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_aad - Error allocating resources for aad_b64");
          ret = RHN_ERROR_MEMORY;
        }
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_aad - Error allocating resources for aad");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jwe->aad = NULL;
      jwe->aad_len = 0;
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}