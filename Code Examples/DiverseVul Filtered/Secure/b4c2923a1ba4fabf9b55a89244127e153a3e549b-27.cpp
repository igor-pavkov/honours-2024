const unsigned char * r_jwe_get_cypher_key(jwe_t * jwe, size_t * key_len) {
  if (jwe != NULL) {
    if (key_len != NULL) {
      *key_len = jwe->key_len;
    }
    return jwe->key;
  }
  return NULL;
}