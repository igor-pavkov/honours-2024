int r_jwe_generate_cypher_key(jwe_t * jwe) {
  int ret;

  if (jwe != NULL && jwe->enc != R_JWA_ENC_UNKNOWN) {
    o_free(jwe->encrypted_key_b64url);
    jwe->encrypted_key_b64url = NULL;
    jwe->key_len = _r_get_key_size(jwe->enc);
    o_free(jwe->key);
    if (!jwe->key_len) {
      ret = RHN_ERROR_PARAM;
    } else if ((jwe->key = o_malloc(jwe->key_len)) != NULL) {
      if (!gnutls_rnd(GNUTLS_RND_KEY, jwe->key, jwe->key_len)) {
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_cypher_key - Error gnutls_rnd");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_cypher_key - Error allocating resources for key");
      ret = RHN_ERROR_MEMORY;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_cypher_key - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}