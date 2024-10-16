int r_jwe_advanced_parse(jwe_t * jwe, const char * jwe_str, uint32_t parse_flags, int x5u_flags) {
  return r_jwe_advanced_parsen(jwe, jwe_str, o_strlen(jwe_str), parse_flags, x5u_flags);
}