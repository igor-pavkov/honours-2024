json_t * r_jwe_get_full_unprotected_header_json_t(jwe_t * jwe) {
  if (jwe != NULL) {
    return _r_json_get_full_json_t(jwe->j_unprotected_header);
  }
  return NULL;
}