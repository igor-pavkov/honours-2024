int r_jwe_parse_json_str(jwe_t * jwe, const char * jwe_json_str, int x5u_flags) {
  return r_jwe_parsen_json_str(jwe, jwe_json_str, o_strlen(jwe_json_str), x5u_flags);
}