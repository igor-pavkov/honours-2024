int r_jwe_advanced_parse_json_str(jwe_t * jwe, const char * jwe_json_str, uint32_t parse_flags, int x5u_flags) {
  return r_jwe_advanced_parsen_json_str(jwe, jwe_json_str, o_strlen(jwe_json_str), parse_flags, x5u_flags);
}