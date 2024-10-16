jwks_t * r_jwe_get_jwks_privkey(jwe_t * jwe) {
  if (jwe != NULL) {
    return r_jwks_copy(jwe->jwks_privkey);
  } else {
    return NULL;
  }
}