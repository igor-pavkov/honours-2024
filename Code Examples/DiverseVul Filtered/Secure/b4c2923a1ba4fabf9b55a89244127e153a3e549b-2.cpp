int r_jwe_generate_iv(jwe_t * jwe) {
  int ret;
  unsigned char * iv_b64 = NULL;
  size_t iv_b64_len = 0;

  if (jwe != NULL && jwe->enc != R_JWA_ENC_UNKNOWN) {
    o_free(jwe->iv_b64url);
    jwe->iv_b64url = NULL;
    jwe->iv_len = gnutls_cipher_get_iv_size(_r_get_alg_from_enc(jwe->enc));
    o_free(jwe->iv);
    jwe->iv = NULL;
    if (jwe->iv_len) {
      if ((jwe->iv = o_malloc(jwe->iv_len)) != NULL) {
        if (!gnutls_rnd(GNUTLS_RND_NONCE, jwe->iv, jwe->iv_len)) {
          if ((iv_b64 = o_malloc(jwe->iv_len*2)) != NULL) {
            if (o_base64url_encode(jwe->iv, jwe->iv_len, iv_b64, &iv_b64_len)) {
              jwe->iv_b64url = (unsigned char *)o_strndup((const char *)iv_b64, iv_b64_len);
              ret = RHN_OK;
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_iv - Error o_base64url_encode iv_b64");
              ret = RHN_ERROR;
            }
            o_free(iv_b64);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_iv - Error allocating resources for iv_b64");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_iv - Error gnutls_rnd");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_generate_iv - Error allocating resources for iv");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jwe->iv_b64url = (unsigned char *)o_strdup("");
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}