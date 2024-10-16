jwe_t * r_jwe_quick_parse(const char * jwe_str, uint32_t parse_flags, int x5u_flags) {
  return r_jwe_quick_parsen(jwe_str, o_strlen(jwe_str), parse_flags, x5u_flags);
}