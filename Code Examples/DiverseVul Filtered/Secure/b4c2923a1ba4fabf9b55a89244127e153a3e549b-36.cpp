int r_jwe_add_key_symmetric(jwe_t * jwe, const unsigned char * key, size_t key_len) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_key = NULL;

  if (jwe != NULL && key != NULL && key_len) {
    if (r_jwk_init(&j_key) == RHN_OK && r_jwk_import_from_symmetric_key(j_key, key, key_len) == RHN_OK) {
      if (r_jwks_append_jwk(jwe->jwks_privkey, j_key) != RHN_OK || r_jwks_append_jwk(jwe->jwks_pubkey, j_key) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_add_enc_key_symmetric - Error setting key");
        ret = RHN_ERROR;
      }
      if (jwe->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_key, "alg"))) != R_JWA_ALG_NONE) {
        r_jwe_set_alg(jwe, alg);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_add_enc_key_symmetric - Error parsing key");
      ret = RHN_ERROR;
    }
    r_jwk_free(j_key);
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}