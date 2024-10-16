int r_jwe_init(jwe_t ** jwe) {
  int ret;

  if (jwe != NULL) {
    if ((*jwe = o_malloc(sizeof(jwe_t))) != NULL) {
      if (((*jwe)->j_header = json_object()) != NULL) {
        if (r_jwks_init(&(*jwe)->jwks_pubkey) == RHN_OK) {
          if (r_jwks_init(&(*jwe)->jwks_privkey) == RHN_OK) {
            (*jwe)->header_b64url = NULL;
            (*jwe)->encrypted_key_b64url = NULL;
            (*jwe)->iv_b64url = NULL;
            (*jwe)->aad_b64url = NULL;
            (*jwe)->ciphertext_b64url = NULL;
            (*jwe)->auth_tag_b64url = NULL;
            (*jwe)->j_unprotected_header = NULL;
            (*jwe)->alg = R_JWA_ALG_UNKNOWN;
            (*jwe)->enc = R_JWA_ENC_UNKNOWN;
            (*jwe)->key = NULL;
            (*jwe)->key_len = 0;
            (*jwe)->iv = NULL;
            (*jwe)->iv_len = 0;
            (*jwe)->aad = NULL;
            (*jwe)->aad_len = 0;
            (*jwe)->payload = NULL;
            (*jwe)->payload_len = 0;
            (*jwe)->j_json_serialization = NULL;
            (*jwe)->token_mode = R_JSON_MODE_COMPACT;
            ret = RHN_OK;
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_init - Error allocating resources for jwks_privkey");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_init - Error allocating resources for jwks_pubkey");
          ret = RHN_ERROR_MEMORY;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_init - Error allocating resources for j_header");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_init - Error allocating resources for jwe");
      ret = RHN_ERROR_MEMORY;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  if (ret != RHN_OK && jwe != NULL) {
    r_jwe_free(*jwe);
    *jwe = NULL;
  }
  return ret;
}