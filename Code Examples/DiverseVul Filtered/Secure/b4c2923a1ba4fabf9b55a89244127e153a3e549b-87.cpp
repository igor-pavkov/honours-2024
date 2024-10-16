int r_jwe_decrypt_key(jwe_t * jwe, jwk_t * jwk_s, int x5u_flags) {
  int ret;
  jwk_t * jwk = NULL;

  if (jwe != NULL) {
    if (jwk_s != NULL) {
      jwk = r_jwk_copy(jwk_s);
    } else {
      if (r_jwe_get_header_str_value(jwe, "kid") != NULL) {
        jwk = r_jwks_get_by_kid(jwe->jwks_privkey, r_jwe_get_header_str_value(jwe, "kid"));
      } else if (r_jwks_size(jwe->jwks_privkey) == 1) {
        jwk = r_jwks_get_at(jwe->jwks_privkey, 0);
      }
    }
  }

  if (jwe != NULL && jwe->alg != R_JWA_ALG_UNKNOWN && jwe->alg != R_JWA_ALG_NONE) {
    ret = r_preform_key_decryption(jwe, jwe->alg, jwk, x5u_flags);
  } else {
    ret = RHN_ERROR_PARAM;
  }

  r_jwk_free(jwk);
  return ret;
}