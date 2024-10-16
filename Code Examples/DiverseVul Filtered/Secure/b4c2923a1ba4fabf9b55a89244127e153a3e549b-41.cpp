jwks_t * r_jwe_get_jwks_pubkey(jwe_t * jwe) {
  if (jwe != NULL) {
    return r_jwks_copy(jwe->jwks_pubkey);
  } else {
    return NULL;
  }
}