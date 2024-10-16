int r_jwe_set_full_header_json_str(jwe_t * jwe, const char * str_header) {
  int ret;
  json_t * j_header = json_loads(str_header, JSON_DECODE_ANY, NULL);

  ret = r_jwe_set_full_header_json_t(jwe, j_header);
  json_decref(j_header);

  return ret;
}