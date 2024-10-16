int r_jwe_set_cypher_key(jwe_t * jwe, const unsigned char * key, size_t key_len) {
  int ret;

  if (jwe != NULL) {
    o_free(jwe->key);
    if (key != NULL && key_len) {
      if ((jwe->key = o_malloc(key_len)) != NULL) {
        memcpy(jwe->key, key, key_len);
        jwe->key_len = key_len;
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_cypher_key - Error allocating resources for key");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jwe->key = NULL;
      jwe->key_len = 0;
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}