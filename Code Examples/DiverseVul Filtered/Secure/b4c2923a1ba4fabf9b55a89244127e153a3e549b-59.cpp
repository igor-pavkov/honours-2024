const char * r_jwe_get_header_str_value(jwe_t * jwe, const char * key) {
  if (jwe != NULL) {
    return _r_json_get_str_value(jwe->j_header, key);
  }
  return NULL;
}