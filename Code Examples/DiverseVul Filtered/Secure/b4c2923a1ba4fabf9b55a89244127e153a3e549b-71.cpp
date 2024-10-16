jwa_enc r_jwe_get_enc(jwe_t * jwe) {
  if (jwe != NULL) {
    return jwe->enc;
  } else {
    return R_JWA_ENC_UNKNOWN;
  }
}