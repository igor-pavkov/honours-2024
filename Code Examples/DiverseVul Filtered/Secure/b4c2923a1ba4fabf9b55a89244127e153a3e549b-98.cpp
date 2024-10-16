int r_jwe_set_enc(jwe_t * jwe, jwa_enc enc) {
  int ret = RHN_OK;

  if (jwe != NULL) {
    jwe->enc = enc;
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}