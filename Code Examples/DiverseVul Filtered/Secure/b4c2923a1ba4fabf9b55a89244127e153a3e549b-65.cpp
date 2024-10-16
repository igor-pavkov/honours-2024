int r_jwe_set_alg(jwe_t * jwe, jwa_alg alg) {
  int ret = RHN_OK;

  if (jwe != NULL) {
    jwe->alg = alg;
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}