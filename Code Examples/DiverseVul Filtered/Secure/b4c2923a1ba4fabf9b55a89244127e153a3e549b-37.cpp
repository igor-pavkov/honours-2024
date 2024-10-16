const unsigned char * r_jwe_get_iv(jwe_t * jwe, size_t * iv_len) {
  if (jwe != NULL) {
    if (iv_len != NULL) {
      *iv_len = jwe->iv_len;
    }
    return jwe->iv;
  }
  return NULL;
}