int r_jwe_parse_json_t(jwe_t * jwe, json_t * jwe_json, int x5u_flags) {
  return r_jwe_advanced_parse_json_t(jwe, jwe_json, R_PARSE_HEADER_ALL, x5u_flags);
}