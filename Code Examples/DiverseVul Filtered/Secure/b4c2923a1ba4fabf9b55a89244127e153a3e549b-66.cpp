jwa_alg r_jwe_get_alg(jwe_t * jwe) {
  if (jwe != NULL) {
    return jwe->alg;
  } else {
    return R_JWA_ALG_UNKNOWN;
  }
}