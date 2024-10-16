int r_jwe_set_full_unprotected_header_json_str(jwe_t * jwe, const char * str_unprotected_header) {
  int ret;
  json_t * j_unprotected_header = json_loads(str_unprotected_header, JSON_DECODE_ANY, NULL);

  ret = r_jwe_set_full_unprotected_header_json_t(jwe, j_unprotected_header);
  json_decref(j_unprotected_header);

  return ret;
}