json_t * r_jwe_get_header_json_t_value(jwe_t * jwe, const char * key) {
  if (jwe != NULL) {
    return _r_json_get_json_t_value(jwe->j_header, key);
  }
  return NULL;
}