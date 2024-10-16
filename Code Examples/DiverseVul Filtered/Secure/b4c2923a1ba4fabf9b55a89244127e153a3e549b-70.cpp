const unsigned char * r_jwe_get_aad(jwe_t * jwe, size_t * aad_len) {
  if (jwe != NULL) {
    if (aad_len != NULL) {
      *aad_len = jwe->aad_len;
    }
    return jwe->aad;
  }
  return NULL;
}