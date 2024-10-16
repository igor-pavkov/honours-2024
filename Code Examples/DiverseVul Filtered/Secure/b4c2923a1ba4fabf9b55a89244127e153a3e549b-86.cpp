int r_jwe_add_keys(jwe_t * jwe, jwk_t * jwk_privkey, jwk_t * jwk_pubkey) {
  int ret = RHN_OK;
  jwa_alg alg;

  if (jwe != NULL && (jwk_privkey != NULL || jwk_pubkey != NULL)) {
    if (jwk_privkey != NULL) {
      if (r_jwks_append_jwk(jwe->jwks_privkey, jwk_privkey) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_add_keys - Error setting jwk_privkey");
        ret = RHN_ERROR;
      }
      if (jwe->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk_privkey, "alg"))) != R_JWA_ALG_NONE) {
        r_jwe_set_alg(jwe, alg);
      }
    }
    if (jwk_pubkey != NULL) {
      if (r_jwks_append_jwk(jwe->jwks_pubkey, jwk_pubkey) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_add_keys - Error setting jwk_pubkey");
        ret = RHN_ERROR;
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}