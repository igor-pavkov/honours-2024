char * r_jwe_serialize(jwe_t * jwe, jwk_t * jwk_pubkey, int x5u_flags) {
  char * jwe_str = NULL;
  int res = RHN_OK;
  unsigned int bits = 0;
  unsigned char * key = NULL;
  size_t key_len = 0;

  if (jwk_pubkey != NULL && jwe != NULL && jwe->alg == R_JWA_ALG_DIR) {
    if (r_jwk_key_type(jwk_pubkey, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC && bits == _r_get_key_size(jwe->enc)*8) {
      key_len = (size_t)(bits/8);
      if ((key = o_malloc(key_len+4)) != NULL) {
        if (r_jwk_export_to_symmetric_key(jwk_pubkey, key, &key_len) == RHN_OK) {
          res = r_jwe_set_cypher_key(jwe, key, key_len);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error r_jwk_export_to_symmetric_key");
          res = RHN_ERROR_MEMORY;
        }
        o_free(key);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error allocating resources for key");
        res = RHN_ERROR_MEMORY;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error invalid key type");
      res = RHN_ERROR_PARAM;
    }
  } else {
    res = RHN_OK;
  }

  if (res == RHN_OK) {
    if (jwe->key == NULL || !jwe->key_len) {
      if (r_jwe_generate_cypher_key(jwe) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error r_jwe_generate_cypher_key");
        res = RHN_ERROR;
      }
    }
    if (jwe->iv == NULL || !jwe->iv_len) {
      if (r_jwe_generate_iv(jwe) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error r_jwe_generate_iv");
        res = RHN_ERROR;
      }
    }
  }
  if (res == RHN_OK && r_jwe_set_alg_header(jwe, jwe->j_header) == RHN_OK && r_jwe_encrypt_key(jwe, jwk_pubkey, x5u_flags) == RHN_OK && r_jwe_encrypt_payload(jwe) == RHN_OK) {
    jwe_str = msprintf("%s.%s.%s.%s.%s",
                      jwe->header_b64url,
                      jwe->encrypted_key_b64url!=NULL?(const char *)jwe->encrypted_key_b64url:"",
                      jwe->iv_b64url,
                      jwe->ciphertext_b64url,
                      jwe->auth_tag_b64url);

  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize - Error input parameters");
  }
  return jwe_str;
}