rhn_int_t r_jwe_get_header_int_value(jwe_t * jwe, const char * key) {
  if (jwe != NULL) {
    return _r_json_get_int_value(jwe->j_header, key);
  }
  return 0;
}